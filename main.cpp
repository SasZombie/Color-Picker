//Color Picker
#include <iostream>
#include <raylib.h>
#include <string>
#include <random>
#include <tuple>
#include <sstream>
#include <unordered_map>

struct Button
{
    Rectangle buttonShape;
    Color col;
};

struct textAreaRGB
{
    Rectangle textAreaShape;
    bool editMode = false;
    short input = 0;
};


struct Slider
{
    Rectangle rec;
    Color col;
    short Position;
    
};


std::unordered_map<char, int> hexMap{{'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};

int hexToRGB(int input);
const char* ShowHex(int input);
double normalize(double value, double min, double max);
void checkColision(textAreaRGB& area);
std::tuple<int, int, int> hslToRGB(int Hue, double Sat, double Light);
std::tuple<int, int, int> randomColor();
void DrawCustomLines(Rectangle& rec);
std::tuple<int, double, double> rgbToHSL(int Red, int Green, int Blue);
void setAreaHsl(textAreaRGB area[], int a, float b, float c);
void checkAreaRGB(textAreaRGB &areaRGB, textAreaRGB &areaHEX, textAreaRGB areaHSL[] , Slider &curSlide, int index, textAreaRGB areaRGBP[]);
void colorButtons(Button buttons[], int NROFBUTTONS);



bool flagShowButton = true;


double normalize(double value, double min, double max)
{
    if(value < min)
        return 0.0f;
    if(value > max)
        return 0.0f;

    double normValue = (value - min)/(max - min);
    return normValue;    
}


void checkColision(textAreaRGB& area)
{
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if(CheckCollisionPointRec(GetMousePosition(), area.textAreaShape))
        {
            area.editMode = true;
            flagShowButton = false;
        }
        else
            area.editMode = false;
    }

}


std::tuple<int, double, double> rgbToHSL(int Red, int Green, int Blue)
{
    double R = static_cast<double>(Red)/255, G = static_cast<double>(Green)/255, B = static_cast<double>(Blue)/255;

    double max = std::max(R, std::max(G, B)), min = std::min(R, std::min(G, B));
    double delta = max - min;

    double hue;
    double sat, light;

    if(std::fabs(delta-0.0f) < 0.005f)
        hue = 0;
    else
        if(std::fabs(max-R) < 0.005f)
            hue = std::fmod((G-B)/delta,6);
        else if(std::fabs(max-G) < 0.005f)
            {
                hue = (B-R)/delta + 2;
            }
            else
                hue = (R - G)/delta + 4;
    
    
    hue = hue * 60;
    
    light = (max + min)/2;
    
    sat = delta/(1- std::abs(2 * light -1));

    return std::make_tuple(std::ceil(hue), sat * 100, light * 100);
}

std::tuple<int, int, int> hslToRGB(int Hue, double Sat, double Light)
{
    Sat = normalize(Sat, 0.0, 100);
    Light = normalize(Light, 0.0, 100);
    double C = static_cast<double>(1.0 - std::abs(2.0*Light - 1.0)) * Sat;

    
    double X = C * (1.0 - std::abs((std::fmod(static_cast<double>(Hue)/60, 2) - 1)));

    double m = Light - C/2.0;

    int value;
    if(Hue == 360)
        value = 360;
    else
        for (int i = 360; i > 0; i = i - 60)
        {
            if(Hue%i == Hue)
                value = i;
        }

    float R = 0, G = 0, B = 0;

    switch (value)
    {
    case 60:
        R = C;
        G = X;
        break;

    case 120:
        R = X;
        G = C;

        break;
    case 180:
        G = C;
        B = X;
        break;
    case 240:
        G = X;
        B = C;
        break;
    case 300:
        R = X;
        B = C;
        break;
    case 360:
        R = C;
        B = X;
        break;
    }

    return std::make_tuple(std::ceil((R+m)*255), std::ceil((G+m)*255), std::ceil((B+m)*255));

}


void setAreaHsl(textAreaRGB area[], int a, float b, float c)
{
    area[0].input = a;
    area[1].input = b;
    area[2].input = c;
}


void checkAreaRGB(textAreaRGB &areaRGB, textAreaRGB &areaHEX, textAreaRGB areaHSL[] , Slider &curSlide, int index, textAreaRGB areaRGBP[])
{

    checkColision(areaRGB);
    checkColision(areaHEX);
    for(short i = 0; i < 3; ++i)
        checkColision(areaHSL[i]);
   
    if(areaRGB.editMode)
    {
        int key = GetKeyPressed();

        if(key == KEY_BACKSPACE)
        {
            areaRGB.input = areaRGB.input / 10;
        }
        else
            if(key >= 48 && key <= 57)
                areaRGB.input = areaRGB.input * 10 + (key - '0');
            

        if(areaRGB.input > 255)
            areaRGB.input = 255;

        areaHEX.input = std::stoi(ShowHex(areaRGB.input), nullptr, 16);
        curSlide.Position = curSlide.rec.x + areaRGB.input;


        auto [R, G, B] = rgbToHSL(areaRGBP[0].input, areaRGBP[1].input, areaRGBP[2].input);

        areaHSL[0].input = R;
        areaHSL[1].input = G;
        areaHSL[2].input = B;

 

    }
    else if(areaHEX.editMode)
    {
        int key = GetKeyPressed();
 
        
        if(key == KEY_BACKSPACE)
        {
            areaHEX.input = areaHEX.input / 10;
        }
        else
            if((key>= 48 && key <= 57) || (key >= 65 && key <= 90)  || (key >= 97 && key <= 122))
            {
                std::stringstream stream;
                stream<<std::hex<<areaHEX.input;

                std::string val = stream.str();
                val = val + static_cast<char>(key);

                areaHEX.input = std::stoi(val, nullptr, 16);               
            }
        
        areaRGB.input = hexToRGB(areaHEX.input);
        if(areaRGB.input > 255)
            areaRGB.input = 255;
        curSlide.Position = curSlide.rec.x + areaRGB.input;

        auto [R, G, B] = rgbToHSL(areaRGBP[0].input, areaRGBP[1].input, areaRGBP[2].input);

        areaHSL[0].input = R;
        areaHSL[1].input = G;
        areaHSL[2].input = B;

   }
    else
        for (short i = 0; i < 3; ++i)
        {
            if(areaHSL[i].editMode)
            {
                int key = GetKeyPressed();

                if(key == KEY_BACKSPACE)
                {
                    areaHSL[i].input = areaHSL[i].input / 10;
                }
                else
                    if(key >= 48 && key <= 57)
                    {
                        areaHSL[i].input = areaHSL[i].input * 10 + (key - '0');
                    }


                if(index == 0 )
                {
                    if(areaHSL[index].input > 360)
                            areaHSL[index].input = 360;
                }
                else
                    if(areaHSL[index].input > 100)
                        areaHSL[index].input = 100;


                auto [R, G, B] = hslToRGB(areaHSL[0].input, areaHSL[1].input, areaHSL[2].input);
                switch (index)
                {
                case 0:
                    areaRGB.input = R;
                    break;
                case 1:
                    areaRGB.input = G;
                    break;
                case 2:
                    areaRGB.input = B;
                    break;
                }
                
                curSlide.Position = curSlide.rec.x + areaRGB.input;

                areaHEX.input = std::stoi(ShowHex(areaRGB.input), nullptr, 16);
                curSlide.Position = curSlide.rec.x + areaRGB.input;
            }

        } 
}   




std::tuple<int, int, int> randomColor()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distribion(0, 256);

    return std::make_tuple(distribion(gen), distribion(gen), distribion(gen));
}

void DrawCustomLines(Rectangle& rec)
{
    DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, WHITE);
}

void colorButtons(Button buttons[], int NROFBUTTONS)
{

    for(short i = 0; i < NROFBUTTONS; ++i)
    {
        auto [R, G, B] = randomColor();
        buttons[i].col = {static_cast<unsigned char>(R), static_cast<unsigned char>(G), static_cast<unsigned char>(B), 0xFF};
    }

}

const char* ShowHex(int input)
{
    std::stringstream stream;

    stream << std::hex << std::uppercase << input;

    return stream.str().c_str();
}


int hexToRGB(int input)
{
    const char* hexValue = ShowHex(input);
    int colorShade = 0;
    char num1 = hexValue[0];
    char num2 = hexValue[1];

    if(std::isdigit(num1))
        colorShade = colorShade + (num1 - '0') * 16;
    else
        colorShade = colorShade + hexMap[num1] * 16;

    if(std::isdigit(num2))
        colorShade = colorShade + (num2 - '0');
    else
        colorShade = colorShade + hexMap[num2];

    return colorShade;

}

int main()
{
    static constexpr int HEIGHT = 500, WIDTH = 600, NROFBUTTONS = 10, BUTTSPACE = 45;


    static constexpr Color BGCOLOR(47, 54, 51, 0xFF);

    Color selectedColor = BGCOLOR;


    InitWindow(WIDTH, HEIGHT, "Color Picker");
    SetTargetFPS(60);
    
    textAreaRGB RGB[3];
    textAreaRGB HEX[3];
    textAreaRGB HSL[3];
    Slider sliders[3];
    Button buttons[NROFBUTTONS];

    Button randomColorsButton;

    for(short i = 0; i < NROFBUTTONS/2; ++i)
    {
        buttons[i].buttonShape = {static_cast<float>(25 + i * BUTTSPACE), 25 , 25, 25};
    }


    for(short i = 0; i < NROFBUTTONS; ++i)
    {
        buttons[i+NROFBUTTONS/2].buttonShape = {static_cast<float>(25 + i * BUTTSPACE), 75 , 25, 25};
    }  
    
    colorButtons(buttons, NROFBUTTONS);
    
    randomColorsButton.buttonShape = {25, 125, 205 , 25};
    randomColorsButton.col = BLACK;


    for(short i = 0; i < 3; ++i)
    {
        sliders[i].rec = {WIDTH/2 + 30, static_cast<float>(HEIGHT/2) + 50 + i * 25, 255, 15};
        sliders[i].Position = sliders[i].rec.x;
        RGB[i].textAreaShape = {WIDTH/2 + 25.0f + i * 75, 125, 75, 25};
        HEX[i].textAreaShape = {WIDTH/2 + 25.0f + i * 75, 75, 75, 25};
        HSL[i].textAreaShape = {WIDTH/2 + 25.0f + i * 75, 25, 75, 25};
    }
   
    sliders[0].col = RED;
    sliders[1].col = GREEN;
    sliders[2].col = BLUE;

    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BGCOLOR);
        
        DrawRectangle(0, HEIGHT/2, WIDTH/2, HEIGHT/2, selectedColor );

        DrawRectangleRec(randomColorsButton.buttonShape, randomColorsButton.col);
        DrawCustomLines(randomColorsButton.buttonShape);
        DrawText("New Random Colors", randomColorsButton.buttonShape.x + 5, randomColorsButton.buttonShape.y + 2, 20, WHITE);

        if(CheckCollisionPointRec(GetMousePosition(), randomColorsButton.buttonShape))
        {
           if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                colorButtons(buttons, NROFBUTTONS);
            }
        }

        for(short i = 0; i < 3; ++i)
        {
            
            DrawRectangleRec(sliders[i].rec, sliders[i].col);
            DrawCustomLines(sliders[i].rec);
            DrawCircle(sliders[i].Position, sliders[i].rec.y + 7, 8, BLACK);
            checkAreaRGB(RGB[i], HEX[i], HSL, sliders[i], i, RGB);


            if(CheckCollisionPointRec(GetMousePosition(), sliders[i].rec))
            {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    float PozX = GetMousePosition().x;
                    sliders[i].Position = PozX;
                    RGB[i].input = sliders[i].Position - 329;
                    HEX[i].input = std::stoi(ShowHex(RGB[i].input), nullptr, 16);

                    auto [R, G, B] = rgbToHSL(RGB[0].input, RGB[1].input, RGB[2].input);

                    HSL[0].input = R;
                    HSL[1].input = G;
                    HSL[2].input = B;
                    flagShowButton = false;

                }
            }


            DrawRectangleRec(RGB[i].textAreaShape, BLACK);
            DrawRectangleRec(HEX[i].textAreaShape, BLACK);
            DrawRectangleRec(HSL[i].textAreaShape, BLACK);
            DrawCustomLines(HEX[i].textAreaShape);
            DrawCustomLines(RGB[i].textAreaShape);
            DrawCustomLines(HSL[i].textAreaShape);

            DrawText(std::to_string(RGB[i].input).c_str(), RGB[i].textAreaShape.x + 10, RGB[i].textAreaShape.y + 4, 20, WHITE);
            DrawText(ShowHex(HEX[i].input), HEX[i].textAreaShape.x + 10, HEX[i].textAreaShape.y + 4, 20, WHITE);
            DrawText(std::to_string(HSL[i].input).c_str(), HSL[i].textAreaShape.x + 10, HSL[i].textAreaShape.y + 4, 20, WHITE);

        }

      


        if(flagShowButton == false)
        {
            selectedColor = {static_cast<unsigned char>(RGB[0].input), static_cast<unsigned char>(RGB[1].input), 
                                                                                        static_cast<unsigned char>(RGB[2].input), 0xFF};
        

        }

        
        for(short i = 0; i < NROFBUTTONS; ++i)
        {
            DrawRectangleRec(buttons[i].buttonShape, buttons[i].col);
            DrawCustomLines(buttons[i].buttonShape);

            if(CheckCollisionPointRec(GetMousePosition(), buttons[i].buttonShape))
            {
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    flagShowButton = true;
                    selectedColor = buttons[i].col;
                    
                    RGB[0].input = selectedColor.r;
                    RGB[1].input = selectedColor.g;
                    RGB[2].input = selectedColor.b;
                    sliders[0].Position = sliders[0].rec.x + RGB[0].input;
                    sliders[1].Position = sliders[1].rec.x + RGB[1].input;
                    sliders[2].Position = sliders[2].rec.x + RGB[2].input;

                    HEX[0].input = std::stoi(ShowHex(RGB[0].input), nullptr, 16);
                    HEX[1].input = std::stoi(ShowHex(RGB[1].input), nullptr, 16);
                    HEX[2].input = std::stoi(ShowHex(RGB[2].input), nullptr, 16);

                    auto [R, G, B] = rgbToHSL(RGB[0].input, RGB[1].input, RGB[2].input);

                    HSL[0].input = R;
                    HSL[1].input = G;
                    HSL[2].input = B;
                }
            }
        }
        

        EndDrawing();
    }
    
    CloseWindow();
}
