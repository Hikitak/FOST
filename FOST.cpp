#include <windows.h>
#include <math.h>
const int NUM = 70; // количество точек
LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
double** x; // массив данных
      // Задание исходных данных для графика
      // (двумерный массив, может содержать несколько рядов данных)
double** getData(int n)
{
    double** f;
    f = new double* [2];
    f[0] = new double[n];
    f[1] = new double[n];
    for (int i = 0; i < n; i++)
    {
        double x = (double)i * 0.1;
        f[0][i] = x;
        f[1][i] = sin(x);
    }
    return f;
}
// Функция рисования графика
void DrawGraph(HDC hdc, RECT rectClient,
    double** x, // массив данных
    int n, // количество точек
    int numrow = 1) // количество рядов данных (по умолчанию 1)
{
    double OffsetY, OffsetX;
    double MAX_X, MAX_Y;
    double ScaleX, ScaleY;
    double min, max;
    int height, width;
    int X, Y; // координаты в окне (в px)
    HPEN hpen;
    height = rectClient.bottom - rectClient.top;
    width = rectClient.right-  rectClient.left;
    // Область допустимых значений X
    min = x[0][0];
    max = x[0][0];
    for (int i = 0; i < n; i++)
    {
        if (x[0][i] < min) min = x[0][i];
        if (x[0][i] > max) max = x[0][i];
    }
    double temp = max - min;
    MAX_X = max - min;
    OffsetX = min * width / MAX_X; // смещение X
    ScaleX = (double)width / MAX_X; // масштабный коэффициент X
                    // Область допустимых значений Y
    min = x[1][0];
    max = x[1][0];
    for (int i = 0; i < n; i++)
    {
        for (int j = 1; j <= numrow; j++)
        {
            if (x[j][i] < min) min = x[j][i];
            if (x[j][i] > max) max = x[j][i];
        }
    }
    MAX_Y = max - min;
    OffsetY = max * height / (MAX_Y); // смещение Y
    ScaleY = (double)height / MAX_Y; // масштабный коэффициент Y
                     // Отрисовка осей координат
    hpen = CreatePen(PS_SOLID, 0, 0); // черное перо 1px
    SelectObject(hdc, hpen);
    MoveToEx(hdc, 0, OffsetY, 0); // перемещение в точку (0;OffsetY)
    LineTo(hdc, width, OffsetY); // рисование горизонтальной оси
    MoveToEx(hdc, OffsetX, 0, 0); // перемещение в точку (OffsetX;0)
    LineTo(hdc, OffsetX, height); // рисование вертикальной оси (не видна)
    DeleteObject(hpen); // удаление черного пера
               // Отрисовка графика функции
    int color = 0xFF; // красное перо для первого ряда данных
    for (int j = 1; j <= numrow; j++)
    {
        hpen = CreatePen(PS_SOLID, 2, color); // формирование пера 2px
        SelectObject(hdc, hpen);
        X = (int)(OffsetX + x[0][0] * ScaleX); // начальная точка графика
        Y = (int)(OffsetY - x[j][0] * ScaleY);
        MoveToEx(hdc, X, Y, 0); // перемещение в начальную точку
        for (int i = 0; i < n; i++)
        {
            X = OffsetX + x[0][i] * ScaleX;
            Y = OffsetY - x[j][i] * ScaleY;
            LineTo(hdc, X, Y);
        }
        color = color << 8; // изменение цвета пера для следующего ряда
        DeleteObject(hpen); // удаление текущего пера
    }
}
// Главная функция
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASS w;
    x = getData(NUM); // задание исходны данных
    memset(&w, 0, sizeof(WNDCLASS));
    w.style = CS_HREDRAW | CS_VREDRAW;
    w.lpfnWndProc = WndProc;
    w.hInstance = hInstance;
    w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
    w.lpszClassName = "My Class";
    RegisterClass(&w);
    hwnd = CreateWindow("My Class", "График функции",
        WS_OVERLAPPEDWINDOW,
        500, 300, 500, 380, NULL, NULL,
        hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
// Оконная функция
LONG WINAPI WndProc(HWND hwnd, UINT Message,
    WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    switch (Message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        DrawGraph(hdc, ps.rcPaint, x, NUM); // построение графика
                          // Вывод текста y=sin(x)
        SetTextColor(hdc, 0x00FF0000); // синий цвет букв
        TextOut(hdc, 10, 20, "y=sin(x)", 8);
        EndPaint(hwnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, Message, wparam, lparam);
    }
    return 0;
}