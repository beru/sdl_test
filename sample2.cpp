#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define MAX_PARTICLE_COUNT 512


SDL_Surface* screen = NULL;                 /* 画面 */
SDL_Surface* background_image = NULL;       /* 背景画像 */
SDL_Surface* particle_image = NULL;         /* 粒子の画像 */


/* 粒子 */
typedef struct {
    double x, y;        /* 座標 */
    double vx, vy;      /* 速度 */
    int alive;          /* 0: 死亡, 1: 生存 */
    int type;           /* 0: 小さい粒子, 1: 大きい粒子 */
    double frame;       /* アニメーションは今何フレーム目か */
} Particle;

/* 粒子の配列 */
Particle particles[MAX_PARTICLE_COUNT];


/* lower 以上 upper 以下の乱数を返す */
double Random(double lower, double upper)
{
   return ((double)rand() / RAND_MAX) * (upper - lower) + lower;
}


/* 粒子を作る。
 * 実際には粒子の配列から死んでる粒子を一個見つけてきて
 * それを生き返らせて、パラメーターを設定する。
 */
int CreateParticle(double x, double y, double vx, double vy, int type)
{
    int i;

    /* 死んでる粒子を探す */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i) {
        if (!particles[i].alive)
            break;
    }
    if (i < MAX_PARTICLE_COUNT) {   /* 見つかった */
        particles[i].alive = 1;
        particles[i].x = x;
        particles[i].y = y;
        particles[i].vx = vx;
        particles[i].vy = vy;
        particles[i].type = type;
        particles[i].frame = 0;
        return i;
    } else {                        /* 見つからなかった */
        return -1;
    }
}


/* index番目の粒子を更新する */
void UpdateParticle(int index)
{
    particles[index].vy += 0.4;
    particles[index].x += particles[index].vx;
    particles[index].y += particles[index].vy;

    particles[index].frame += 0.5;
    if (particles[index].frame >= 4)
        particles[index].frame = 0;

    /* 画面下に出ていたら死亡 */
    if (particles[index].y > 480)
        particles[index].alive = 0;
}


/* 更新する */
void Update(void)
{
    int i;

    /* 生きている全粒子を更新する */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i) {
        if (particles[i].alive)
            UpdateParticle(i);
    }

    CreateParticle(320, 240, Random(-5, 5), Random(-8, -16), 0);
    CreateParticle(312, 232, Random(-5, 5), Random(-8, -16), 1);
}


/* 粒子を描画する */
void DrawParticle(int index)
{
    SDL_Rect srcrect;
    SDL_Rect destrect = { particles[index].x, particles[index].y };

    switch (particles[index].type) {
    case 0:
        srcrect.x = (int)particles[index].frame * 16;
        srcrect.y = 0;
        srcrect.w = srcrect.h = 16;
        break;
    case 1:
        srcrect.x = (int)particles[index].frame * 32;
        srcrect.y = 16;
        srcrect.w = srcrect.h = 32;
        break;
    }

    SDL_BlitSurface(particle_image, &srcrect, screen, &destrect);
}


/* 描画する */
void Draw(void)
{
    int i;

    /* 背景を描画する */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* 生きている全粒子を描画する */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i) {
        if (particles[i].alive)
            DrawParticle(i);
    }

    /* 画面を更新する */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


/* 初期化する。
 * 成功したときは0を、失敗したときは-1を返す。
 */
int Initialize(void)
{
    int i;

    /* SDLを初期化する */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDLの初期化に失敗しました：%s\n", SDL_GetError());
        return -1;
    }
    /* 画面のタイトルを変更する */
    SDL_WM_SetCaption("My SDL Sample Game", NULL);
    /* 画面を初期化する */
    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    if (screen == NULL) {
        fprintf(stderr, "画面の初期化に失敗しました：%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    /* 画像を読み込む */
    background_image = IMG_Load("background2.png");
    if (background_image == NULL) {
        fprintf(stderr, "画像の読み込みに失敗しました：%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    particle_image = IMG_Load("particle.png");
    if (particle_image == NULL) {
        fprintf(stderr, "画像の読み込みに失敗しました：%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    /* 乱数を初期化する */
    srand(time(NULL));

    /* 粒子の配列を初期化する */
    for (i = 0; i < MAX_PARTICLE_COUNT; ++i)
        particles[i].alive = 0;

    return 0;
}


/* メインループ */
void MainLoop(void)
{
    SDL_Event event;
    double next_frame = SDL_GetTicks();
    double wait = 1000.0 / 60;

    for (;;) {
        /* すべてのイベントを処理する */
        while (SDL_PollEvent(&event)) {
            /* QUIT イベントが発生するか、ESC キーが押されたら終了する */
            if ((event.type == SDL_QUIT) ||
                (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
                return;
        }
        /* 1秒間に60回Updateされるようにする */
        if (SDL_GetTicks() >= next_frame) {
            Update();
            /* 時間がまだあるときはDrawする */
            if (SDL_GetTicks() < next_frame + wait)
                Draw();
            next_frame += wait;
            SDL_Delay(0);
        }
    }
}


/* 終了処理を行う */
void Finalize(void)
{
    SDL_FreeSurface(background_image);
    SDL_FreeSurface(particle_image);

    /* 終了する */
    SDL_Quit();
}


/* メイン関数 */
int main(int argc, char* argv[])
{
    if (Initialize() < 0)
        return -1;
    MainLoop();
    Finalize();
    return 0;
}
