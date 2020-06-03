#include <windows.h>
#include <gl/gl.h>

#define SCREEN_WIDTH 15
#define SCREEN_HEIGHT 15

#define SCREEN_AMOUNT (SCREEN_WIDTH * SCREEN_HEIGHT)

float piece_width = 2.0f / SCREEN_WIDTH;
float piece_height = 2.0f / SCREEN_HEIGHT;
int ix = 0;
int iy = -1;

struct dots_t {short x; short y;};

dots_t snake[SCREEN_AMOUNT];
dots_t food;
int snake_size = 1;


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
void newFood();
void newGame();


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    unsigned int move_time = 0;
    float esq = -1.0f;
    float top = 1.0f;
    float px;
    float py;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          256,
                          256,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    newGame();
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
          // movimentar a cobra
          if (GetTickCount() - move_time > 330) {
              for (int i = SCREEN_AMOUNT - 1; i > 0; --i) {
                snake[i].x = snake[i-1].x;
                snake[i].y = snake[i-1].y;
              }
              snake[0].x += ix;
              snake[0].y += iy;
              if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH ||
                  snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) {
                newGame();
              }
              if (snake[0].x == food.x && snake[0].y == food.y) {
                if (snake_size + 1 < SCREEN_AMOUNT)  {
                  ++snake_size;
                  newFood();
                } else {
                  newGame();
                }
              } else {
                for (int i = 1; i < snake_size; ++i) {
                  if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    newGame();
                  }
                }
              }
              move_time = GetTickCount();
            }

            // desenhar os objetos
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            glBegin(GL_QUADS);
              glColor3f(0.95f,0.95f,0.95f);
              for (int y = 0; y < SCREEN_WIDTH; ++y) {
                for (int x = 0; x < SCREEN_WIDTH; ++x) {
                  if (x % 2 != 0 || y % 2 != 0)
                    continue;
                   px = esq + x * piece_width;
                   py = top - y * piece_height;
                   glVertex2f(px, py);
                   glVertex2f(px + piece_width, py);
                   glVertex2f(px + piece_width, py - piece_height);
                   glVertex2f(px , py - piece_height);
                }
              }
              glColor3f(0.2f,0.2f,0.2f);
              px = esq + food.x * piece_width;
              py = top - food.y * piece_height;
              glVertex2f(px, py);
              glVertex2f(px + piece_width, py);
              glVertex2f(px + piece_width, py - piece_height);
              glVertex2f(px , py - piece_height);
              glColor3f(0.2f,0.2f,1.0f);
              for (int i = 0; i < snake_size; ++i) {
                if (i == 1) {
                  glColor3f(0.6f,0.7f,0.0f);
                }
                px = esq + snake[i].x * piece_width;
                py = top - snake[i].y * piece_height;
                glVertex2f(px, py);
                glVertex2f(px + piece_width, py);
                glVertex2f(px + piece_width, py - piece_height);
                glVertex2f(px , py - piece_height);
              }
            glEnd();
            glPopMatrix();

            SwapBuffers(hDC);

            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_UP:
                  if (iy == 0) {
                    ix = 0;
                    iy = -1;
                  }
                break;
                case VK_DOWN:
                  if (iy == 0) {
                    ix = 0;
                    iy = 1;
                  }
                break;
                case VK_RIGHT:
                  if (ix == 0) {
                    ix = 1;
                    iy = 0;
                  }
                break;
                case VK_LEFT:
                  if (ix == 0) {
                    ix = -1;
                    iy = 0;
                  }
                break;
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

void newGame() {
  snake_size = 1;
  snake[0].x = (int)(SCREEN_WIDTH / 2);
  snake[0].y = (int)(SCREEN_HEIGHT / 2);
  newFood();
}


void newFood() {
  dots_t empty_places[SCREEN_AMOUNT];
  int place_count = 0;
  for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
      empty_places[place_count].x = x;
      empty_places[place_count].y = y;
      ++place_count;
    }
  }
  for (int i = 0; i < snake_size; ++i) {
    for (int j = 0; j < place_count; ++j) {
      if (empty_places[j].x == snake[i].x && empty_places[j].y == snake[i].y) {
        for (int k = j; k + 1 < place_count; ++k)
          empty_places[k] = empty_places[k + 1];
        --place_count;
        break;
      }
    }
  }
  if (place_count < 1)
    return;
  int choose = rand() % place_count;
  food.x = empty_places[choose].x;
  food.y = empty_places[choose].y;
}

