#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Container/MVector.h"
#include "/home/codeleaded/System/Static/Container/Pair.h"
#include "/home/codeleaded/System/Static/Library/TransformedView.h"
#include "/home/codeleaded/System/Static/Container/SQuadTree.h"


#define WORLDSIZE_X			20
#define WORLDSIZE_Y			20

#define TILE_SIZE			1.0f
#define ASPECTRATIO			2.0f

#define WORLD_NONE			255
#define WORLD_CLEAR			0
#define WORLD_FOCUS			1
#define WORLD_GRAS			2
#define WORLD_SAND			3
#define WORLD_TREECLEAR		4
#define WORLD_TREEOAK		5

typedef unsigned char Block;

Block world[WORLDSIZE_Y * WORLDSIZE_X];
Vector assets;

Block Isometric_World_Get(Block* world,Vec2 p){
	if(p.x<0.0f || p.x>=WORLDSIZE_X || p.y<0.0f || p.y>=WORLDSIZE_Y) return WORLD_NONE;
	return world[(int)p.y * WORLDSIZE_X + (int)p.x];
}
void Isometric_World_Set(Block* world,Vec2 p,Block b){
	if(p.x<0.0f || p.x>=WORLDSIZE_X || p.y<0.0f || p.y>=WORLDSIZE_Y) return;
	world[(int)p.y * WORLDSIZE_X + (int)p.x] = b;
}

Sprite* Isometric_World_GetSprite(Block* world,Vec2 p){
	Block b = Isometric_World_Get(world,p);
	if(b!=WORLD_NONE && b>=0 && b<assets.size)
		return (Sprite*)Vector_Get(&assets,b);
	return NULL;
}

Vec2 Isometric_World_Screen_Pos(Vec2 p){
	Vec2 pos;
	pos.x = 0.5f * (p.x - p.y);
	pos.y = 0.25f * (p.x + p.y);
	return pos;
}
Vec2 Isometric_Screen_World_Pos(Vec2 p){
	Vec2 pos;
	pos.x = p.x + 2.0f * p.y;
	pos.y = 4.0f * (p.y - 0.25f * (p.x + 2.0f * p.y));
	return pos;
}



TransformedView tv;

void Setup(AlxWindow* w){
	tv = TransformedView_New((Vec2){ GetWidth() * 0.15f,GetHeight() * 0.15f });
	tv.Offset = (Vec2){ 0.0f,0.0f };

	memset(world,0,sizeof(Block) * WORLDSIZE_X * WORLDSIZE_Y);

	assets = Vector_New(sizeof(Sprite));
	Vector_Push(&assets,(Sprite[]){ Sprite_Load("./assets/Clear.png") });
	Vector_Push(&assets,(Sprite[]){ Sprite_Load("./assets/Focus.png") });
	Vector_Push(&assets,(Sprite[]){ Sprite_Load("./assets/Gras.png") });
	Vector_Push(&assets,(Sprite[]){ Sprite_Load("./assets/Sand.png") });
	Vector_Push(&assets,(Sprite[]){ Sprite_Load("./assets/Tree_Clear.png") });
	Vector_Push(&assets,(Sprite[]){ Sprite_Load("./assets/Tree_Oak.png") });
}

void Update(AlxWindow* w){
    TransformedView_HandlePanZoom(&tv,window.Strokes,(Vec2){ GetMouse().x,GetMouse().y });
	

	const Vec2 mouse_screen = TransformedView_ScreenWorldPos(&tv,GetMouse());
	const Vec2 mouse_world = Vec2_Func(Vec2_Add(Isometric_Screen_World_Pos(mouse_screen),(Vec2){ -1.5f,-0.5f }),F32_Floor);

	const Vec2 mouse_back_world = Isometric_World_Screen_Pos((Vec2){ mouse_world.x,mouse_world.y });
	const Vec2 mouse_back_screen = TransformedView_WorldScreenPos(&tv,mouse_back_world);

	if(Stroke(ALX_MOUSE_L).PRESSED){
		Block b = Isometric_World_Get(world,mouse_world);
		if(b!=WORLD_NONE)
			Isometric_World_Set(world,mouse_world,(b < WORLD_TREEOAK ? b + 1 : 0));
	}


	for(int i = 0;i<assets.size;i++){
		Sprite* sp = (Sprite*)Vector_Get(&assets,i);

		const Vec2 length = TransformedView_WorldScreenLength(&tv,(Vec2){ TILE_SIZE,TILE_SIZE * ASPECTRATIO });
		Sprite_Reload(sp,(int)length.x,(int)length.y);
	}

	Clear(LIGHT_BLUE);
	
	const Vec2 leftTop = TransformedView_ScreenWorldPos(&tv,(Vec2){ GetWidth() * 0.5f,GetHeight() * -0.25f });
	const Vec2 rightBottom = TransformedView_ScreenWorldPos(&tv,(Vec2){ GetWidth() * 0.5f,GetHeight() * 1.25f });
	const Vec2 im_leftTop = Isometric_Screen_World_Pos(leftTop);
	const Vec2 im_rightBottom = Isometric_Screen_World_Pos(rightBottom);

	for(int i = 0;i<WORLDSIZE_Y;i++){
		for(int j = 0;j<WORLDSIZE_X;j++){
	//for(int i = (int)im_leftTop.y;i<(int)im_rightBottom.y;i++){
	//	for(int j = (int)im_leftTop.x;j<(int)im_rightBottom.x;j++){
			const Vec2 pos_isometric = (Vec2){ j,i };
			const Vec2 pos_world = Isometric_World_Screen_Pos(pos_isometric);
			const Vec2 pos_screen = TransformedView_WorldScreenPos(&tv,pos_world);

			Sprite* sp = Isometric_World_GetSprite(world,pos_isometric);
			if(sp){
				RenderSpriteAlpha(sp,pos_screen.x,pos_screen.y - sp->h / ASPECTRATIO);
			}
		}
	}


	
	

	Sprite* mouse_sp = (Sprite*)Vector_Get(&assets,WORLD_FOCUS);
	if(mouse_sp){
		RenderSpriteAlpha(mouse_sp,mouse_back_screen.x,mouse_back_screen.y - mouse_sp->h / ASPECTRATIO);
	}

	String str = String_Format("X:%d, Y:%d",(Number)mouse_world.x,(Number)mouse_world.y);
	RenderCStrSize(str.Memory,str.size,0.0f,0.0f,RED);
	String_Free(&str);
}

void Delete(AlxWindow* w){
	for(int i = 0;i<assets.size;i++){
		Sprite* sp = (Sprite*)Vector_Get(&assets,i);
		Sprite_Free(sp);
	}
    Vector_Free(&assets);
}

int main(){
    if(Create("Isometric Tiles",2500,1300,1,1,Setup,Update,Delete))
        Start();
    return 0;
}