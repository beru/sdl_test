#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>


/* 入力データ */
typedef struct {
    int left;
    int up;
    int right;
    int down;
    int button1;
    int button2;
} InputData;


InputData current_input, prev_input;        /* 入力データ */

SDL_Surface* screen = NULL;                 /* 画面 */
SDL_Surface* background_image = NULL;       /* 背景画像 */
SDL_Surface* onpu_image = NULL;             /* 音符 */

double x, y;                                /* 音符の座標 */


/* 入力データを更新する */
void UpdateInput(void)
{
    Uint8* keys = SDL_GetKeyState(NULL);

    prev_input = current_input;
    current_input.left = keys[SDLK_LEFT] | keys[SDLK_h];        /* [←], [H] */
    current_input.up = keys[SDLK_UP] | keys[SDLK_k];            /* [↑], [K] */
    current_input.right = keys[SDLK_RIGHT] | keys[SDLK_l];      /* [→], [L] */
    current_input.down = keys[SDLK_DOWN] | keys[SDLK_j];        /* [↓], [J] */
    current_input.button1 = keys[SDLK_LSHIFT] | keys[SDLK_z];   /* 左Shift, [Z] */
    current_input.button2 = keys[SDLK_LCTRL] | keys[SDLK_x];    /* 左Ctrl, [X] */
}


/* 更新する */
void Update(void)
{
    int speed = 6;

    UpdateInput();

    /* ボタン1が押されているときは低速 */
    if (current_input.button1)
        speed = 2;
    /* ボタン2が押されたら(0, 0)に移動 */
    if (current_input.button2 && !prev_input.button2)   /* 押された瞬間のみ */
        x = y = 0;

    /* 入力にしたがって移動する */
    if (current_input.left)
        x -= speed;
    if (current_input.right)
        x += speed;
    if (current_input.up)
        y -= speed;
    if (current_input.down)
        y += speed;

    /* 画面外に出ないようにする */
    if (x < 0)
        x = 0;
    else if (x > 640 - 64)
        x = 640 - 64;
    if (y < 0)
        y = 0;
    else if (y > 480 - 64)
        y = 480 - 64;
}


/* 描画する */
void Draw(void)
{
    SDL_Rect destrect = { x, y };

    /* 背景を描画する */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* 音符を描画する */
    SDL_BlitSurface(onpu_image, NULL, screen, &destrect);

    /* 画面を更新する */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


/* 初期化する。
 * 成功したときは0を、失敗したときは-1を返す。
 */
int Initialize(void)
{
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
    background_image = IMG_Load("background.png");
    if (background_image == NULL) {
        fprintf(stderr, "画像の読み込みに失敗しました：%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    onpu_image = IMG_Load("onpu.png");
    if (onpu_image == NULL) {
        fprintf(stderr, "画像の読み込みに失敗しました：%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

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
    SDL_FreeSurface(onpu_image);

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
