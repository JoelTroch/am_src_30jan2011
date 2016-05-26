#include "hud.h"
#include "cl_util.h"
#include "triangleapi.h"

void BallCollide( void );
void ResetGame( void );

/////////////////////////////////////
// Global array for a single stage //
// just an example to start with   //
/////////////////////////////////////

int Stage[1][10][10]=  //one stage with a 10x10 grid
{   
    1,2,3,4,5,5,4,3,2,1,
    2,1,2,3,4,4,3,2,1,2,
    3,2,1,2,3,3,2,1,2,3,
    4,3,2,1,2,2,1,2,3,4,
    5,4,3,2,1,1,2,3,4,5,
    5,4,3,2,1,1,2,3,4,5,
    4,3,2,1,2,2,1,2,3,4,
    3,2,1,2,3,3,2,1,2,3,
    2,1,2,3,4,4,3,2,1,2,
    1,2,3,4,5,5,4,3,2,1
};
//each constant will act as a different color for the blocks
// a constant of 0 will mean the block has been destroyed

float ball_x,ball_y;//ball position
float sp_x,sp_y;//ball speed
float ballsize;

void CHud::InitGame( void )
{
    gHUD.blocksize_x = ScreenWidth/12;
    gHUD.blocksize_y = ScreenHeight/20;
    gHUD.batsize_x = ScreenWidth/13;
    gHUD.batsize_y = ScreenHeight/40;
    bat_x= ScreenWidth/2;
    bat_y= ScreenHeight - gHUD.blocksize_y;
    ballsize = ScreenWidth /100;
    sp_x = 80;sp_y =-80;
    ball_x = bat_x+ballsize;
    ball_y = (bat_y - (ballsize*5));
}

void CHud::UpdateGame( void )
{
    //add the speed to the ball//
    ball_x += gHUD.m_flTimeDelta * sp_x;
    ball_y += gHUD.m_flTimeDelta * sp_y;
    
    BallCollide();
}

void CHud::RenderGame( void )
{
    ////Draw the bat////

    gEngfuncs.pTriAPI->SpriteTexture( 
        (struct model_s *)gEngfuncs.GetSpritePointer(SPR_Load("sprites/dot.spr")) , 
        0); 
    gEngfuncs.pTriAPI->CullFace( TRI_NONE ); 
    gEngfuncs.pTriAPI->Begin(TRI_QUADS);
    gEngfuncs.pTriAPI->RenderMode(kRenderNormal); 
    gEngfuncs.pTriAPI->Color4f(1,1,1,1);
    gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);
    gEngfuncs.pTriAPI->Vertex3f(bat_x, bat_y, 0); //top left
    gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);
    gEngfuncs.pTriAPI->Vertex3f(bat_x, bat_y - batsize_y, 0); //bottom left
    gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f);
    gEngfuncs.pTriAPI->Vertex3f(bat_x + batsize_x , bat_y - batsize_y, 0); //bottom right
    gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f);
    gEngfuncs.pTriAPI->Vertex3f(bat_x + batsize_x, bat_y, 0); //top right 
    gEngfuncs.pTriAPI->End(); 
    ////Draw the blocks////
    gEngfuncs.pTriAPI->SpriteTexture( 
        (struct model_s *)gEngfuncs.GetSpritePointer(SPR_Load("sprites/vp_parallel.spr")) , 
        0);
    for(int y=0;y<10;y++){
        for(int x=0;x<10;x++){
            gEngfuncs.pTriAPI->CullFace( TRI_NONE ); 
            gEngfuncs.pTriAPI->Begin(TRI_QUADS);
            gEngfuncs.pTriAPI->RenderMode(kRenderNormal); 
            switch(Stage[0][y][x])
            {
            case 1: gEngfuncs.pTriAPI->Color4f(1,0,0,1);break;
            case 2: gEngfuncs.pTriAPI->Color4f(1,1,0,1);break;
            case 3: gEngfuncs.pTriAPI->Color4f(0,1,0,1);break;
            case 4: gEngfuncs.pTriAPI->Color4f(0,1,1,1);break;
            case 5: gEngfuncs.pTriAPI->Color4f(0,0,1,1);break;
            case 6: gEngfuncs.pTriAPI->Color4f(1,0,1,1);break;
            case 7: gEngfuncs.pTriAPI->Color4f(0.5f,0,0,1);break;
            case 8: gEngfuncs.pTriAPI->Color4f(0,0.5f,0,1);break;
            case 9: gEngfuncs.pTriAPI->Color4f(0,0,0.5f,1);break;
            }
            if(Stage[0][y][x]!=0)
            {//only render if its not been hit
                gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);
                gEngfuncs.pTriAPI->Vertex3f(blocksize_x*(x+1), blocksize_y*(y+2), 0); //top left
                gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);
                gEngfuncs.pTriAPI->Vertex3f(blocksize_x*(x+1), (blocksize_y*(y+2)) - blocksize_y, 0); //bottom left
                gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f);
                gEngfuncs.pTriAPI->Vertex3f((blocksize_x*(x+1)) + blocksize_x , (blocksize_y*(y+2)) - blocksize_y, 0); //bottom right
                gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f);
                gEngfuncs.pTriAPI->Vertex3f((blocksize_x*(x+1)) + blocksize_x, blocksize_y*(y+2), 0); //top right 
            }
            gEngfuncs.pTriAPI->End(); 
            gEngfuncs.pTriAPI->RenderMode(kRenderNormal); 
        }
    }
    ////Draw the ball////
    gEngfuncs.pTriAPI->SpriteTexture( 
        (struct model_s *)gEngfuncs.GetSpritePointer(SPR_Load("sprites/dot.spr")) , 
        0); //use hotglow, or any other sprite for the texture
    gEngfuncs.pTriAPI->CullFace( TRI_NONE ); 
    gEngfuncs.pTriAPI->Begin(TRI_QUADS);
    gEngfuncs.pTriAPI->RenderMode(kRenderNormal); 
    gEngfuncs.pTriAPI->Color4f(1,1,1,1);
    gEngfuncs.pTriAPI->TexCoord2f(0.0f, 1.0f);
    gEngfuncs.pTriAPI->Vertex3f(ball_x, ball_y, 0); //top left
    gEngfuncs.pTriAPI->TexCoord2f(0.0f, 0.0f);
    gEngfuncs.pTriAPI->Vertex3f(ball_x, ball_y - ballsize, 0); //bottom left
    gEngfuncs.pTriAPI->TexCoord2f(1.0f, 0.0f);
    gEngfuncs.pTriAPI->Vertex3f(ball_x + ballsize , ball_y - ballsize, 0); //bottom right
    gEngfuncs.pTriAPI->TexCoord2f(1.0f, 1.0f);
    gEngfuncs.pTriAPI->Vertex3f(ball_x + ballsize, ball_y, 0); //top right  
    gEngfuncs.pTriAPI->End(); 
}

void BallCollide( void )
{
    bool Hitx =false,Hity=false;//hit flag
    bool BlocksLeft=false;
    //check collision with block//
    for(int y=0;y<10;y++)
    {
        for(int x=0;x<10;x++)
        {
            if(Stage[0][y][x]!=0){
                //save this for later//
                //there is still block(s) left//
                BlocksLeft=true;
                //check right side//
                if((ball_x+(ballsize/2) > (gHUD.blocksize_x *(x+1))+(gHUD.blocksize_x-(gHUD.blocksize_x/10))) && 
                    (ball_x+(ballsize/2) < (gHUD.blocksize_x *(x+1))+gHUD.blocksize_x) &&
                    (ball_y-(ballsize/2) < gHUD.blocksize_y *(y+2)) &&
                    (ball_y-(ballsize/2) > (gHUD.blocksize_y *(y+2))-gHUD.blocksize_y))
                {
                    Stage[0][y][x] = 0;
                    Hitx=true;
                }
                //check left side//
                if((ball_x+(ballsize/2) > gHUD.blocksize_x *(x+1)) && 
                    (ball_x+(ballsize/2) < (gHUD.blocksize_x *(x+1))+(gHUD.blocksize_x/10)) &&
                    (ball_y-(ballsize/2) < gHUD.blocksize_y *(y+2)) &&
                    (ball_y-(ballsize/2) > (gHUD.blocksize_y *(y+2))-gHUD.blocksize_y))
                {
                    Stage[0][y][x] = 0;
                    Hitx=true;
                }
                //check bottom side//
                if((ball_x+(ballsize/2) > gHUD.blocksize_x *(x+1)) && 
                    (ball_x+(ballsize/2) < (gHUD.blocksize_x *(x+1))+gHUD.blocksize_x) &&
                    (ball_y-(ballsize/2) < gHUD.blocksize_y *(y+2)) &&
                    (ball_y-(ballsize/2) > (gHUD.blocksize_y *(y+2))-(gHUD.blocksize_y/10)))
                {
                    Stage[0][y][x] = 0;
                    Hity=true;
                }
                //check top side//
                if((ball_x+(ballsize/2) > gHUD.blocksize_x *(x+1)) && 
                    (ball_x+(ballsize/2) < (gHUD.blocksize_x *(x+1))+gHUD.blocksize_x) &&
                    (ball_y-(ballsize/2) > (gHUD.blocksize_y *(y+2))-gHUD.blocksize_y) &&
                    (ball_y-(ballsize/2) < (gHUD.blocksize_y *(y+2))-(gHUD.blocksize_y-(gHUD.blocksize_y/10))))
                {
                    Stage[0][y][x] = 0;
                    Hity=true;
                }
            }
        }
    }
    //check bat//
    //leftside
    if(ball_x+(ballsize/2) > gHUD.bat_x && ball_x+(ballsize/2) < gHUD.bat_x+(gHUD.batsize_x/4) &&
        ball_y-(ballsize/2) > gHUD.bat_y-gHUD.batsize_y && ball_y-(ballsize/2) < gHUD.bat_y)
    {
        Hity=true;
        sp_x>0?Hitx=true,sp_x+=5,sp_y+=5:sp_x-=5,sp_y-=5;
    }
    else if(ball_x+(ballsize/2) > gHUD.bat_x+(gHUD.batsize_x/4) && ball_x+(ballsize/2) < gHUD.bat_x+(gHUD.batsize_x/2) &&
        ball_y-(ballsize/2) > gHUD.bat_y-gHUD.batsize_y && ball_y-(ballsize/2) < gHUD.bat_y)
    {
        Hity=true;
        sp_x>0?Hitx=true,sp_x+=10,sp_y+=10:sp_x-=10,sp_y-=10;
    }
    //rightside
    if(ball_x+(ballsize/2) < gHUD.bat_x+(gHUD.batsize_x-(gHUD.batsize_x/4)) && ball_x+(ballsize/2) > gHUD.bat_x+(gHUD.batsize_x/2) &&
        ball_y-(ballsize/2) > gHUD.bat_y-gHUD.batsize_y && ball_y-(ballsize/2) < gHUD.bat_y)
    {
        Hity=true;
        sp_x<0?Hitx=true,sp_x-=5,sp_y-=5:sp_x+=5,sp_y+=5;
    }
    else if(ball_x+(ballsize/2) < gHUD.bat_x+gHUD.batsize_x && ball_x+(ballsize/2) > gHUD.bat_x+(gHUD.batsize_x-(gHUD.batsize_x/4)) &&
        ball_y-(ballsize/2) > gHUD.bat_y-gHUD.batsize_y && ball_y-(ballsize/2) < gHUD.bat_y)
    {
        Hity=true;
        sp_x<0?Hitx=true,sp_x-=10,sp_y-=10:sp_x+=10,sp_y+=10;
    }
    
    
    if(Hitx)
        sp_x -= ((sp_x)*2);
    if(Hity)
        sp_y -= ((sp_y)*2);
    
    //check edges of screen//
    if(ball_x<gHUD.blocksize_x)
        sp_x -= (sp_x*2);
    if(ball_x+(ballsize*2)>ScreenWidth-gHUD.blocksize_x)
        sp_x -= (sp_x*2);
    if(ball_y-(ballsize*2)<0+gHUD.blocksize_y)
        sp_y -= (sp_y*2);
    if(ball_y-(ballsize)>ScreenHeight)
        ResetGame();
    
    //Any blocks left?//
    if(!BlocksLeft)//you could call the next level here//
        ResetGame();
}

void ResetGame( void )
{
    gHUD.blocksize_x = ScreenWidth/12;
    gHUD.blocksize_y = ScreenHeight/20;
    gHUD.batsize_x = ScreenWidth/13;
    gHUD.batsize_y = ScreenHeight/40;
    gHUD.bat_x= ScreenWidth/2;
    gHUD.bat_y= ScreenHeight - gHUD.blocksize_y;
    sp_x = 80;
    sp_y =-80;
    ball_x = gHUD.bat_x+ballsize;
    ball_y = (gHUD.bat_y - (ballsize*5));
    Stage[1][10][10]= 
        1,2,3,4,5,5,4,3,2,1,
        2,1,2,3,4,4,3,2,1,2,
        3,2,1,2,3,3,2,1,2,3,
        4,3,2,1,2,2,1,2,3,4,
        5,4,3,2,1,1,2,3,4,5,
        5,4,3,2,1,1,2,3,4,5,
        4,3,2,1,2,2,1,2,3,4,
        3,2,1,2,3,3,2,1,2,3,
        2,1,2,3,4,4,3,2,1,2,
        1,2,3,4,5,5,4,3,2,1;
}
