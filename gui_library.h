#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define ITEM_OPTION 0
#define ITEM_BUTTON 1
#define ITEM_Label  2
#define ITEM_HGRAPH 3
#define ITEM_ROUNDGAUGE 4
#define ITEM_EDITOR_BUTTON 5
#define ITEM_EDITOR_LABEL 6 //Differs from label becuase this is an editLabel which will spawn a editor on the screen.

#define DIR_LEFT 0
#define DIR_RIGHT 1
#define DIR_UP 2
#define DIR_DOWN 3

/*
Gui library for Adafruit TFT

Gui Elements:
1. Label
  Displays string to user. 

2. Button
  Invokes function when pressed; function is defined as void funcName()

3. Horizontal Gauge
  Shows a value in bar graph format.
*/

class widget{
  public:
  int xpos;
  int ypos;
  char widgetName[10];
  int widgetType = -1; 
  int renderCount = 0; 
  widget(int XPOS, int YPOS)
  {
    xpos = XPOS;
    ypos = YPOS;
  }
};
class menuOption : public widget
{
  public:
  char optionText[20];
  void* linkedPagePTR;
  menuOption(char optionName[]): widget(-1,-1)
  {
    strcpy(optionText,optionName);
    widgetType = ITEM_OPTION;
  }
};

class label : public widget
{
  public:
  char labelValue[20];
  int labelColor = 0;
  void* editPagePointer;
  label(char labelName[]) : widget(-1,-1)
  {
    strcpy(labelValue,labelName);
    widgetType = ITEM_Label;
  }

  label(char labelName[], int x, int y, int Color) : widget(x,y)
  {
    strcpy(labelValue,labelName);
    widgetType = ITEM_Label;
    labelColor = Color;
  }
  void setValue(char newValue[]);
};

class editorLabel : public widget
{
  public:
  char labelValue[20];
  int labelColor = 0;
  void* editPagePointer;  
  float* editorValue = NULL;
  editorLabel(char labelName[], int x, int y, int Color, float* editorVALUE) : widget(x,y)
  {
    strcpy(labelValue,labelName);
    widgetType = ITEM_EDITOR_LABEL;
    editorValue = editorVALUE;
    labelColor = Color;
  }
  void setValue(char newValue[]);
};

class button : public widget{
public:
  void (*fun_ptr)();
  button(char buttonName[], void* FunctionPTR, int XPOS, int YPOS) : widget(XPOS, YPOS)
  {
    widgetType = ITEM_BUTTON;
    memcpy(widgetName,buttonName,10);
    fun_ptr = FunctionPTR;
  }
  void executeHandler()
  {
    (*fun_ptr)();
  }
};

class menuPage
{
  public:
  widget* widget_array[5];
  char title[20];
  int widgetCount = 0;
  int selectedItem = 0;  
  void selectMoveDown();
  void selectMoveUp();
  void setTitle(char newTitle[]);
  void addWidget(widget* inputWidget);
  widget* selectMenuItem();//return a page when widget is clicked
  menuPage()
  {
  
  }

  
  
};

class editPage : public menuPage
{
  public:
  class editorButton : public widget
  {
    void(editPage::*PTR)() = NULL;
    editPage* PTR2 = NULL;
    public:
    editorButton(char buttonName[], editPage* EditPAGE_PTR,void(editPage::*ptr1)(), int XPOS, int YPOS) : widget(XPOS, YPOS)
    {
      widgetType = ITEM_EDITOR_BUTTON;
      memcpy(widgetName,buttonName,10);
      PTR = ptr1;
      PTR2 = EditPAGE_PTR;
    }

    void executeHandler()
    {
      (*PTR2.*PTR)();
    }

  };

   
  float* editPageValue;//There is a value to which the page is linked
  label valueLabel = label("VALUE:",0,25,YELLOW);
  editorButton upValueButton = editorButton("UP",this,&editPage::editPageUPHandler,0,50);
  editorButton downValueButton = editorButton("DOWN",this,&editPage::editPageDOWNHandler,0,75);
  editPage() : menuPage()
  {    
    setTitle("EDITOR");
    //Connect the buttons to their up and down value functions.         
    addWidget(&valueLabel);
    addWidget(&upValueButton);
    addWidget(&downValueButton);
  }

  void setValuePTR(float* newPTR)
  {
    editPageValue = newPTR;
  }

  void editPageUPHandler()
  {
    *editPageValue = *editPageValue + 1;
    Serial.println("UP_VALUE");
    Serial.println(*editPageValue);
    //Update the valueLabel
    char output[20];
    char fValue[16];
    dtostrf(*editPageValue,3,2,fValue);
    strcpy(output,"VALUE:");
    strcat(output,fValue);
    valueLabel.setValue(output);
  }

  void editPageDOWNHandler()
  {
    *editPageValue = *editPageValue - 1;
    //Update the valueLabel
    char output[20];
    char fValue[16];
    dtostrf(*editPageValue,3,2,fValue);
    strcpy(output,"VALUE:");
    strcat(output,fValue);
    valueLabel.setValue(output);
  }
};

class Renderer
{
  private:
  Adafruit_TFTLCD* tftInstance;
  public:
  menuPage* PAGES[5];
  menuPage* currentPage = NULL;
  menuPage* previousPage = NULL;  
  editPage EDITOR = editPage();  
  int pageCount = 0;
  Renderer(Adafruit_TFTLCD* TFTInstance)
  {
    //EDITOR.addWidget(&valueLabel);
    //EDITOR.addWidget(&upValueButton);
    tftInstance = TFTInstance;
   
  }
  void addPage(menuPage* inputPage);
  void selectCurrentOption();
  void moveSelectedItem(int DIRECTION);
  void back();
  void render();
};
