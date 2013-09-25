#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>


SDL_Surface* screen = NULL;                 /* 画面 */
SDL_Surface* background_image = NULL;       /* 背景画像 */
SDL_Surface* letters_image = NULL;          /* 文字画像 */

int counter = 0;                            /* 毎フレーム1づつカウントアップするカウンタ */


/* (x, y)にテキスト(ただし、数字、アルファベット、空白のみ)を表示する。
 * printf と同じような書き方ができる。
 */
void DrawText(int x, int y, const char* format, ...)
{
    int i;
    va_list ap;
    char buffer[256];

    va_start(ap, format);
    vsnprintf(buffer, 255, format, ap);
    va_end(ap);
    buffer[255] = '\0';
    for (i = 0; buffer[i] != '\0'; ++i) {
        SDL_Rect srcrect1, srcrect2;
        SDL_Rect destrect1 = { x + i*10, y };
        SDL_Rect destrect2 = { x + i*10 + 2, y + 2 };

        srcrect1.w = srcrect1.h = srcrect2.w = srcrect2.h = 10;
        if (isdigit(buffer[i])) {           /* 数字 */
            srcrect1.x = (buffer[i] - '0') * 10;
            srcrect1.y = 20;
        } else if (isalpha(buffer[i])) {    /* アルファベット */
            srcrect1.x = (toupper(buffer[i]) - 'A') * 10;
            srcrect1.y = 0;
        } else {                            /* それ以外は空白とみなす */
            continue;
        }
        srcrect2.x = srcrect1.x;
        srcrect2.y = srcrect1.y + 10;
        SDL_BlitSurface(letters_image, &srcrect2, screen, &destrect2);
        SDL_BlitSurface(letters_image, &srcrect1, screen, &destrect1);
    }
}


/* 更新する */
void Update(void)
{
    ++counter;
    if (counter == 60 * 10) {              /* 10秒経ったら終了イベントを発生させる */
        SDL_Event quit_event = { SDL_QUIT };
        SDL_PushEvent(&quit_event);
    }
}


/* 描画する */
void Draw(void)
{
    /* 背景を描画する */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* 文字を60フレームの周期で点滅させて表示する */
    if (counter / 30 % 2 == 0) {
        DrawText(200, 235, "PRESS ESC KEY TO EXIT");
    }
    /* カウントダウンを表示する */
    DrawText(270, 255, "%2d SEC", 10 - counter / 60);

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
    letters_image = IMG_Load("letters.png");
    if (letters_image == NULL) {
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
    SDL_FreeSurface(letters_image);

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
