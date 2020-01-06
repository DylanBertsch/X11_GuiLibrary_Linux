#include <cstdio>
#include "gui_library.h"

void label::setValue(char newValue[])
{
    strcpy(labelValue,newValue);
}

void editorLabel::setValue(char newValue[])
{

}

void menuPage::selectMoveDown()
{
    selectedItem++;
}

void menuPage::selectMoveUp()
{
    selectedItem--;
}

void menuPage::setTitle(char newTitle[])
{
    strcpy(title, newTitle);
}

void menuPage::addWidget(widget* inputWidget)
{
    widget_array[widgetCount] = inputWidget;
    widgetCount++;
}

widget* menuPage::selectMenuItem()
{
    //Get widget from widget array storage
    widget* wid = (widget*)widget_array[selectedItem];
    return wid;
}

void Renderer::addPage(menuPage* inputPage)
{
    if(pageCount == 0)
    {
        currentPage = inputPage;
    }
    PAGES[pageCount] = inputPage;
    pageCount++;
}

void Renderer::selectCurrentOption()
{
    widget* wid = currentPage->selectMenuItem();
    if(wid->widgetType == ITEM_EDITOR_LABEL)//Do edit label things... open edit page.
    {
        editorLabel* lbl = (editorLabel*)wid;
        float* val = lbl->editorValue;
        //Serial.println(*val);
        EDITOR.editPageValue = val;
        previousPage = currentPage;
        //Set the editPage float value; the value we are going to be editing....
        currentPage = &EDITOR;
        XClearWindow(d,*window);
        render();
    }
    if(wid->widgetType == ITEM_OPTION)
    {
        menuOption* option = (menuOption*)wid;
        previousPage = currentPage;
        //currentPage = option->linkedPagePTR;
        //tftInstance->fillScreen(BLACK);
        render();
    }
    if(wid->widgetType == ITEM_BUTTON)
    {
        button* btn = (button*)wid;
        btn->executeHandler();
        render();
    }
    if(wid->widgetType == ITEM_EDITOR_BUTTON)//In the editor context
    {
        //Run the handler function
        editPage::editorButton* editorButton = (editPage::editorButton*)wid;
        editorButton->executeHandler();
        render();
    }
}
void Renderer::back()
{
    currentPage = previousPage;
    render();
}

void Renderer::render()
{
    //Render the pageTitle
    XClearWindow(d,*window);
    XSetForeground(d,*gc,WhitePixel(d,s));
    int titleWidth = XTextWidth(pageFont,currentPage->title,strlen(currentPage->title));
    XDrawString(d,*window,*gc,500-(titleWidth/2),35,currentPage->title,strlen(currentPage->title));
    yCursor = 100;

    for(int index = 0; index < currentPage->widgetCount; index++)
    {
        widget* widPTR = (widget*)currentPage->widget_array[index];
        if(widPTR->widgetType == ITEM_OPTION)
        {
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                XSetForeground(d,*gc,WhitePixel(d,s));
            }
            //Print the Text
            menuOption* option = (menuOption*)currentPage->widget_array[index];
            XDrawString(d,*window,*gc,xCursor,yCursor,option->optionText,strlen(option->optionText));
            XSetForeground(d,*gc,WhitePixel(d,s));//Draw a white line
            XDrawLine(d,*window,*gc,0,yCursor+10,1000,yCursor+10);
            yCursor = yCursor + 65;

        }
        if(widPTR->widgetType == ITEM_Label)
        {
            label* lbl1 = (label*)currentPage->widget_array[index];
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                XSetForeground(d,*gc,WhitePixel(d,s));
            }
            int textWidth = XTextWidth(pageFont,lbl1->labelValue,strlen(lbl1->labelValue));
            //XDrawRectangle(d,*window,*gc,lbl1->xpos,lbl1->ypos,textWidth,50);
            XDrawString(d,*window,*gc,lbl1->xpos,lbl1->ypos+35,lbl1->labelValue,strlen(lbl1->labelValue));
        }
        if(widPTR->widgetType == ITEM_EDITOR_LABEL)
        {
            editorLabel* lbl1 = (editorLabel*)currentPage->widget_array[index];
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                XSetForeground(d,*gc,WhitePixel(d,s));
            }
            char copyBuffer[50];
            char fValue[15];
            sprintf(fValue,"%f",*lbl1->editorValue);
            strcpy(copyBuffer,lbl1->labelValue);
            strcat(copyBuffer,fValue);
            int textWidth = XTextWidth(pageFont,copyBuffer,strlen(copyBuffer));
            //XDrawRectangle(d,*window,*gc,lbl1->xpos,lbl1->ypos,textWidth,50);
            XDrawString(d,*window,*gc,lbl1->xpos,lbl1->ypos+35,copyBuffer,strlen(copyBuffer));
        }
        if(widPTR->widgetType == ITEM_BUTTON)
        {
            button* btn = (button*)currentPage->widget_array[index];
                if(index == currentPage->selectedItem)//Set the text color green to indicate selection
                {
                    setForegroundColor(0,1000000000,0);
                }
                else// Paint white if text is not selected.
                {
                    XSetForeground(d,*gc,WhitePixel(d,s));
                }
                if(btn->xpos > 0 || btn->ypos > 0)//Forced Position bypass vertical layout.
                {
                    int textWidth = XTextWidth(pageFont,btn->widgetName,strlen(btn->widgetName));
                    XDrawRectangle(d,*window,*gc,btn->xpos,btn->ypos,textWidth,50);
                    XDrawString(d,*window,*gc,btn->xpos,btn->ypos+35,btn->widgetName,strlen(btn->widgetName));
                }
                else
                {

                }
        }

        if(widPTR->widgetType == ITEM_EDITOR_BUTTON)
        {
            editPage::editorButton* btn = (editPage::editorButton*)currentPage->widget_array[index];
            if(index == currentPage->selectedItem)//Set the text color green to indicate selection
            {
                setForegroundColor(0,1000000000,0);
            }
            else// Paint white if text is not selected.
            {
                XSetForeground(d,*gc,WhitePixel(d,s));
            }
            if(btn->xpos > 0 || btn->ypos > 0)//Forced Position bypass vertical layout.
            {
                int textWidth = XTextWidth(pageFont,btn->widgetName,strlen(btn->widgetName));
                XDrawRectangle(d,*window,*gc,btn->xpos,btn->ypos,textWidth,50);
                XDrawString(d,*window,*gc,btn->xpos,btn->ypos+35,btn->widgetName,strlen(btn->widgetName));
            }
            else
            {

            }
        }


    }//End the for loop
}

void Renderer::moveSelectedItem(int DIRECTION)
{
    if(DIRECTION == DIR_DOWN)
    {
        currentPage->selectedItem++;
        render();
    }
    if(DIRECTION == DIR_UP)
    {
        currentPage->selectedItem--;
        render();
    }
}
