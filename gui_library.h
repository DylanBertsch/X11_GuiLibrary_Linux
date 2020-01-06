#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

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
Gui library for X11 Window System

Gui Elements:
1. Label
  Displays string to user. 

2. Button
  Invokes function when pressed; function is defined as void funcName()

3. Horizontal Gauge
  Shows a value in bar graph format.
*/

class Widget{
  public:
  int xpos;
  int ypos;
  char widgetName[10];
  int widgetType = -1;
  Widget(int XPOS, int YPOS)
  {
    xpos = XPOS;
    ypos = YPOS;
  }
};
class menuOption : public Widget
{
  public:
  char optionText[20];
  void* linkedPagePTR;
  menuOption(char optionName[]): Widget(-1, -1)
  {
    strcpy(optionText,optionName);
    widgetType = ITEM_OPTION;
  }
};

class HorizontalGraph : public Widget{
public:
    float value = 0.0;
    float min = 0.0;
    float max = 1.0;
    char unitName[10];
    HorizontalGraph(char* graphName,char* unitName, float MIN, float MAX, int XPOS, int YPOS) : Widget(XPOS, YPOS)
    {
        widgetType = ITEM_HGRAPH;
        min = MIN;
        max = MAX;
        strcpy(widgetName,graphName);
        strcpy(unitName, unitName);
    }

    void setValue(float newValue)
    {
        value = newValue;
    }

};


class label : public Widget
{
  public:
  char labelValue[20];
  int labelColor = 0;
  void* editPagePointer;
  label(char labelName[]) : Widget(-1, -1)
  {
    strcpy(labelValue,labelName);
    widgetType = ITEM_Label;
  }

  label(char labelName[], int x, int y, int Color) : Widget(x, y)
  {
    strcpy(labelValue,labelName);
    widgetType = ITEM_Label;
    labelColor = Color;
  }
  void setValue(char newValue[]);
};

class editorLabel : public Widget
{
  public:
  char labelValue[20];
  int labelColor = 0;
  void* editPagePointer;  
  float* editorValue = NULL;
  editorLabel(char labelName[], int x, int y, int Color, float* editorVALUE) : Widget(x, y)
  {
    strcpy(labelValue,labelName);
    widgetType = ITEM_EDITOR_LABEL;
    editorValue = editorVALUE;
    labelColor = Color;
  }
  void setValue(char newValue[]);
};

class button : public Widget{
public:
  void (*fun_ptr)(void);
  button(char buttonName[], void(*function_pointer)(void), int XPOS, int YPOS) : Widget(XPOS, YPOS)
  {
    widgetType = ITEM_BUTTON;
    memcpy(widgetName,buttonName,10);
    fun_ptr = function_pointer;
  }
  void executeHandler()
  {
    (*fun_ptr)();
  }
};

class menuPage
{
  public:
  Widget* widget_array[5];
  char title[20];
  int widgetCount = 0;
  int selectedItem = 0;  
  void selectMoveDown();
  void selectMoveUp();
  void setTitle(char newTitle[]);
  void addWidget(Widget* inputWidget);
  Widget* selectMenuItem();//return a page when Widget is clicked
  menuPage()
  {
  
  }

  
  
};

class editPage : public menuPage
{
  public:
  class editorButton : public Widget
  {
    void(editPage::*PTR)() = NULL;
    editPage* PTR2 = NULL;
    public:
    editorButton(char buttonName[], editPage* EditPAGE_PTR,void(editPage::*ptr1)(), int XPOS, int YPOS) : Widget(XPOS, YPOS)
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
  label valueLabel = label("VALUE:",0,25,0);
  editorButton upValueButton = editorButton("UP",this,&editPage::editPageUPHandler,0,125);
  editorButton downValueButton = editorButton("DOWN",this,&editPage::editPageDOWNHandler,0,250);
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
    //Update the valueLabel
    char output[20];
    char fValue[16];
    sprintf(fValue,"%f",*editPageValue);
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
      sprintf(fValue,"%f",*editPageValue);
      strcpy(output,"VALUE:");
      strcat(output,fValue);
      valueLabel.setValue(output);
  }
};

class Renderer
{
  private:
    //Define the X11 environment variables
    Display *display;
    int s;
    Window* window;
    GC* gc;
    XEvent e;

  public:
  int xCursor = 0;
  int yCursor = 100;
  XFontStruct* pageFont;
  menuPage* PAGES[5];
  menuPage* currentPage = NULL;
  menuPage* previousPage = NULL;  
  editPage EDITOR = editPage();  
  int pageCount = 0;
  Renderer(Display* DISPLAY, Window* inputwindow, GC* graphics_context, int S)
  {
      display = DISPLAY;
      window = inputwindow;
      gc = graphics_context;
      s = S;
      //Set the font
      Font font;
      font = XLoadFont(display, "-b&h-lucida-medium-r-normal-sans-34-240-100-100-p-191-iso8859-1");//xlsfonts
      pageFont = XQueryFont(display, font);
      XSetFont(display, *gc, font);
      //Set the initial text color
      XSetForeground(display, *gc, WhitePixel(display, 0));
  }
  //X11 utilities
  void setForegroundColor(int red, int green, int blue)
  {
      XColor color_cell;
      color_cell.flags = DoRed | DoGreen | DoBlue;
      color_cell.red = red;
      color_cell.green = green;
      color_cell.blue = blue;
      Colormap cmap = XDefaultColormap(display, s);
      XAllocColor(display, cmap, &color_cell);
      XSetForeground(display, *gc, color_cell.pixel);
  }

  void addPage(menuPage* inputPage);
  void selectCurrentOption();
  void moveSelectedItem(int DIRECTION);
  void back();

  void render();
};
