#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <sprig.h>

#include "element.h"

/* ���̓f�[�^ */
typedef struct {
    int left;
    int up;
    int right;
    int down;
    int button1;
    int button2;
} InputData;


InputData current_input, prev_input;        /* ���̓f�[�^ */

SDL_Surface* screen = NULL;                 /* ��� */
SDL_Surface* background_image = NULL;       /* �w�i�摜 */
SDL_Surface* onpu_image = NULL;             /* ���� */
SDL_Surface* letters_image = NULL;          /* �����摜 */

/* (x, y)�Ƀe�L�X�g(�������A�����A�A���t�@�x�b�g�A�󔒂̂�)��\������B
 * printf �Ɠ����悤�ȏ��������ł���B
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
        if (isdigit(buffer[i])) {           /* ���� */
            srcrect1.x = (buffer[i] - '0') * 10;
            srcrect1.y = 20;
        } else if (isalpha(buffer[i])) {    /* �A���t�@�x�b�g */
            srcrect1.x = (toupper(buffer[i]) - 'A') * 10;
            srcrect1.y = 0;
        } else {                            /* ����ȊO�͋󔒂Ƃ݂Ȃ� */
            continue;
        }
        srcrect2.x = srcrect1.x;
        srcrect2.y = srcrect1.y + 10;
        SDL_BlitSurface(letters_image, &srcrect2, screen, &destrect2);
        SDL_BlitSurface(letters_image, &srcrect1, screen, &destrect1);
    }
}

double x, y;                                /* �����̍��W */
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

		/* ������`�悷�� */
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

/* ���̓f�[�^���X�V���� */
void UpdateInput(void)
{
    Uint8* keys = SDL_GetKeyState(NULL);

    prev_input = current_input;
    current_input.left = keys[SDLK_LEFT] | keys[SDLK_h];        /* [��], [H] */
    current_input.up = keys[SDLK_UP] | keys[SDLK_k];            /* [��], [K] */
    current_input.right = keys[SDLK_RIGHT] | keys[SDLK_l];      /* [��], [L] */
    current_input.down = keys[SDLK_DOWN] | keys[SDLK_j];        /* [��], [J] */
    current_input.button1 = keys[SDLK_LSHIFT] | keys[SDLK_z];   /* ��Shift, [Z] */
    current_input.button2 = keys[SDLK_LCTRL] | keys[SDLK_x];    /* ��Ctrl, [X] */
}


/* �X�V���� */
void Update(void)
{
    int speed = 6;

    UpdateInput();

    /* �{�^��1��������Ă���Ƃ��͒ᑬ */
    if (current_input.button1)
        speed = 2;
    /* �{�^��2�������ꂽ��(0, 0)�Ɉړ� */
    if (current_input.button2 && !prev_input.button2)   /* �����ꂽ�u�Ԃ̂� */
        x = y = 0;

    /* ���͂ɂ��������Ĉړ����� */
    if (current_input.left)
        x -= speed;
    if (current_input.right)
        x += speed;
    if (current_input.up)
        y -= speed;
    if (current_input.down)
        y += speed;

    /* ��ʊO�ɏo�Ȃ��悤�ɂ��� */
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


/* �`�悷�� */
void Draw(void)
{
    SDL_Rect destrect = { x, y };

    /* �w�i��`�悷�� */
    SDL_BlitSurface(background_image, NULL, screen, NULL);

    /* ������`�悷�� */
    SDL_BlitSurface(onpu_image, NULL, screen, &destrect);
	
	elementList.Render();

    /* ��ʂ��X�V���� */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}


/* ����������B
 * ���������Ƃ���0���A���s�����Ƃ���-1��Ԃ��B
 */
int Initialize(void)
{
    /* SDL������������ */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL�̏������Ɏ��s���܂����F%s\n", SDL_GetError());
        return -1;
    }
    /* ��ʂ̃^�C�g����ύX���� */
    SDL_WM_SetCaption("My SDL Sample Game", NULL);
    /* ��ʂ����������� */
    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    if (screen == NULL) {
        fprintf(stderr, "��ʂ̏������Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    /* �摜��ǂݍ��� */
    background_image = IMG_Load("background.png");
    if (background_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    onpu_image = IMG_Load("onpu.png");
    if (onpu_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    letters_image = IMG_Load("letters.png");
    if (letters_image == NULL) {
        fprintf(stderr, "�摜�̓ǂݍ��݂Ɏ��s���܂����F%s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
	
	elementList.Add(&b0);
//	elementList.Add(&b1);

    return 0;
}


/* ���C�����[�v */
void MainLoop(void)
{
    SDL_Event event;
    double next_frame = SDL_GetTicks();
    double wait = 1000.0 / 60;

    for (;;) {
        /* ���ׂẴC�x���g���������� */
        while (SDL_PollEvent(&event)) {
            /* QUIT �C�x���g���������邩�AESC �L�[�������ꂽ��I������ */
            if ((event.type == SDL_QUIT) ||
                (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE))
                return;
        }
        /* 1�b�Ԃ�60��Update�����悤�ɂ��� */
        if (SDL_GetTicks() >= next_frame) {
            Update();
            /* ���Ԃ��܂�����Ƃ���Draw���� */
            if (SDL_GetTicks() < next_frame + wait)
                Draw();
            next_frame += wait;
            SDL_Delay(0);
        }
    }
}


/* �I���������s�� */
void Finalize(void)
{
    SDL_FreeSurface(background_image);
    SDL_FreeSurface(onpu_image);
    SDL_FreeSurface(letters_image);

    /* �I������ */
    SDL_Quit();
}


/* ���C���֐� */
int main(int argc, char* argv[])
{
    if (Initialize() < 0)
        return -1;
    MainLoop();
    Finalize();
    return 0;
}
