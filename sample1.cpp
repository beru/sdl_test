#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>


SDL_Surface* screen = NULL;                 /* 画面 */
SDL_Surface* background_image = NULL;       /* 背景画像 */


/* 更新する */
void Update(void)
{
    /* TODO: ここにゲームの状態を更新する処理を書く */
}


/* 描画する */
void Draw(void)
{
    /* 背景を描画する */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* TODO: ここに描画処理を書く */

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

    /* TODO: ここで画像や音声を読み込む */
    background_image = IMG_Load("background.png");
    if (background_image == NULL) {
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
    /* TODO: ここで画像や音声を解放する */
    SDL_FreeSurface(background_image);

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
