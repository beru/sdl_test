#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <sprig.h>

#include "element.h"

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
SDL_Surface* letters_image = NULL;          /* 文字画像 */

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

double x, y;                                /* 音符の座標 */
class Ball : public Element
{
protected:
	virtual void Update()
	{
		pos.x = 300;
		pos.y = 300;

		pivot.x = 30;
		pivot.y = 30;

#if 1
		angle += 2.5;
#else
		if (current_input.left) {
			angle -= 1.0;
		}
		if (current_input.right) {
			angle += 1.0;
		}
#endif
		if (current_input.up) {
			scale.x += 0.01;
			scale.y += 0.01;
		}
		if (current_input.down) {
			scale.x -= 0.01;
			scale.y -= 0.01;
		}
	}

	virtual void Render()
	{
		SDL_Rect destrect = {0};
		Point pt;
		pt.x = 0;
		pt.y = 0;
		pt = mat * pt;
		SPG_Point pts[4];
		pts[0].x = pt.x;
		pts[0].y = pt.y;
		pt.x = onpu_image->w;
		pt.y = 0;
		pt = mat * pt;
		pts[1].x = pt.x;
		pts[1].y = pt.y;
		pt.x = onpu_image->w;
		pt.y = onpu_image->h;
		pt = mat * pt;
		pts[2].x = pt.x;
		pts[2].y = pt.y;
		pt.x = 0;
		pt.y = onpu_image->h;
		pt = mat * pt;
		pts[3].x = pt.x;
		pts[3].y = pt.y;
		SPG_Polygon(screen, 4, pts, 0);
		SPG_TransformX(onpu_image, screen, angle, scale.x, scale.y, pivot.x, pivot.y, pos.x+pivot.x, pos.y+pivot.y, SPG_TBLEND);

		DrawText(10, 10, "%f", angle);

		/* 音符を描画する */
//		SDL_BlitSurface(onpu_image, NULL, screen, &destrect);
	}
	virtual bool HitTest(Point pt)
	{
		return false;
	}
	virtual bool HitTest(Element* e)
	{
		return false;
	}
};

Element* elements[32];
ElementList elementList(elements, 32);
Ball b0;
Ball b1;

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

	Matrix m;
	m.Identity();
	elementList.UpdateMatrix(elements[0], m);
	elementList.Update();
}


/* 描画する */
void Draw(void)
{
    SDL_Rect destrect = { x, y };

    /* 背景を描画する */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* 音符を描画する */
    SDL_BlitSurface(onpu_image, NULL, screen, &destrect);
	
	elementList.Render();

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
    letters_image = IMG_Load("letters.png");
    if (letters_image == NULL) {
        fprintf(stderr, "画像の読み込みに失敗しました：%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
	
	elementList.Add(&b0);
//	elementList.Add(&b1);

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
