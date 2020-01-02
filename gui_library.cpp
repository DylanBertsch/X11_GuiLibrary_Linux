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
    //Set render count of option and button widgets to 0; this will rerender them
    widget* wid;
    for(int i = 0; i < widgetCount; i++)
    {
      wid = widget_array[i];
      if(wid->widgetType == ITEM_OPTION || wid->widgetType == ITEM_BUTTON)
      {
        wid->renderCount = 0;
      }

    }
}

void menuPage::selectMoveUp()
{
    selectedItem--;
    //Set render count of option and button widgets to 0; this will rerender them
    widget* wid;
    for(int i = 0; i < widgetCount; i++)
    {
      wid = widget_array[i];
      if(wid->widgetType == ITEM_OPTION || wid->widgetType == ITEM_BUTTON || wid->widgetType == ITEM_Label)
      {
        wid->renderCount = 0;
      }

    }
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
      Serial.println(*val);
      EDITOR.editPageValue = val;
      previousPage = currentPage;   
      //Set the editPage float value; the value we are going to be editing....
      currentPage = &EDITOR;      
      tftInstance->fillScreen(BLACK); 
      render();
    }
    if(wid->widgetType == ITEM_OPTION)
    {
        menuOption* option = (menuOption*)wid;
        previousPage = currentPage;   
        currentPage = option->linkedPagePTR;
        tftInstance->fillScreen(BLACK); 
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
    tftInstance->fillScreen(BLACK);
    //Reset all widgets on the currentPage
    widget* wid;
    for(int i = 0; i < currentPage->widgetCount; i++)
    {
      wid = currentPage->widget_array[i];
      wid->renderCount = 0;
    }
    render();
}

void Renderer::render()
{
    tftInstance->setTextColor(GREEN,BLACK);
    tftInstance->setTextSize(2);        
    //Render the pageTitle
    tftInstance->setCursor(85,0);
    tftInstance->println(currentPage->title);
    tftInstance->setCursor(0,30);    
    for(int index = 0; index < currentPage->widgetCount; index++)
    {
      widget* widPTR = (widget*)currentPage->widget_array[index];               
      if(widPTR->widgetType == ITEM_OPTION)
      {
        if(index == currentPage->selectedItem)//Paint the text green
        {
          tftInstance->setTextColor(GREEN); 
        }
        else
        {
          tftInstance->setTextColor(WHITE); 
        }
        //Print the Text               
        menuOption* option = (menuOption*)currentPage->widget_array[index];        
        if(option->renderCount == 0)
        {
        tftInstance->println(option->optionText);
        tftInstance->drawLine(0,tftInstance->getCursorY()+2,400,tftInstance->getCursorY()+2,BLUE);
        tftInstance->drawLine(0,tftInstance->getCursorY()+3,400,tftInstance->getCursorY()+3,BLUE);
        tftInstance->setCursor(0,tftInstance->getCursorY()+7);
        tftInstance->setTextColor(WHITE); 
        }
        option->renderCount++;
      } 
      if(widPTR->widgetType == ITEM_Label)
      {        
        label* lbl1 = (label*)currentPage->widget_array[index]; 
        if(index == currentPage->selectedItem)//Paint the text green if selected
        {
          tftInstance->setTextColor(GREEN); 
        }
        else
        {
          tftInstance->setTextColor(lbl1->labelColor,BLACK);
        }
        
        int prevCursorX = tftInstance->getCursorX();         
        int prevCursorY = tftInstance->getCursorY();//Save the previous cursor values.               
        //tftInstance->fillRect(indexedItem->xpos+75,indexedItem->ypos,125,50,RED);
        tftInstance->setCursor(lbl1->xpos,lbl1->ypos);;
        tftInstance->println(lbl1->labelValue);
        tftInstance->setCursor(prevCursorX,prevCursorY);
        lbl1->renderCount++;        
      } 
      if(widPTR->widgetType == ITEM_EDITOR_LABEL)
      {        
        editorLabel* lbl1 = (editorLabel*)currentPage->widget_array[index]; 
        if(index == currentPage->selectedItem)//Paint the text green if selected
        {
          tftInstance->setTextColor(GREEN,BLACK);
        }
        else
        {
          tftInstance->setTextColor(lbl1->labelColor,BLACK);
        }
        
        int prevCursorX = tftInstance->getCursorX();         
        int prevCursorY = tftInstance->getCursorY();//Save the previous cursor values.               
        //tftInstance->fillRect(indexedItem->xpos+75,indexedItem->ypos,125,50,RED);
        tftInstance->setCursor(lbl1->xpos,lbl1->ypos);
        char copyBuffer[50];
        char fValue[15];
        dtostrf(*lbl1->editorValue,3,2,fValue);
        strcpy(copyBuffer,lbl1->labelValue);
        strcat(copyBuffer,fValue);
        tftInstance->println(copyBuffer);                        
        tftInstance->setCursor(prevCursorX,prevCursorY);
        lbl1->renderCount++;        
      } 
      if(widPTR->widgetType == ITEM_BUTTON)
      {
        button* btn = (button*)currentPage->widget_array[index];
        if(btn->renderCount == 0)
        {
          if(index == currentPage->selectedItem)
          {
            tftInstance->setTextColor(GREEN); 
          }
          else
          {
            tftInstance->setTextColor(WHITE); 
          }
          if(btn->xpos > 0 || btn->ypos > 0)//Forced Position bypass vertical layout. 
          {  
            int prevCursorX = tftInstance->getCursorX();         
            int prevCursorY = tftInstance->getCursorY();//Save the previous cursor values.

            tftInstance->setCursor(btn->xpos,btn->ypos);
            tftInstance->println(btn->widgetName);          
            tftInstance->drawRect(btn->xpos,btn->ypos-5,strlen(btn->widgetName)*12,25,YELLOW);        
            //Set to previous cursor values
            tftInstance->setCursor(prevCursorX,prevCursorY);
          }
          else
          {
            tftInstance->setCursor(tftInstance->getCursorX(),tftInstance->getCursorY()+5);
            tftInstance->println(btn->widgetName);
            tftInstance->drawRect(tftInstance->getCursorX(),tftInstance->getCursorY()-20,85,25,WHITE);                  
            tftInstance->setCursor(0,tftInstance->getCursorY()+15);
          }
        }
        btn->renderCount++;
      }

      if(widPTR->widgetType == ITEM_EDITOR_BUTTON)
      {
        editPage::editorButton* btn = (editPage::editorButton*)currentPage->widget_array[index];
        if(index == currentPage->selectedItem)
        {
          tftInstance->setTextColor(GREEN); 
        }
        else
        {
          tftInstance->setTextColor(WHITE); 
        }
        if(btn->xpos > 0 || btn->ypos > 0)//Forced Position bypass vertical layout. 
        {  
          int prevCursorX = tftInstance->getCursorX();         
          int prevCursorY = tftInstance->getCursorY();//Save the previous cursor values.

          tftInstance->setCursor(btn->xpos,btn->ypos);
          tftInstance->println(btn->widgetName);          
          tftInstance->drawRect(btn->xpos,btn->ypos-5,strlen(btn->widgetName)*12,25,YELLOW);        
          //Set to previous cursor values
          tftInstance->setCursor(prevCursorX,prevCursorY);
        }
        else
        {
          tftInstance->setCursor(tftInstance->getCursorX(),tftInstance->getCursorY()+5);
          tftInstance->println(btn->widgetName);
          tftInstance->drawRect(tftInstance->getCursorX(),tftInstance->getCursorY()-20,85,25,WHITE);                  
          tftInstance->setCursor(0,tftInstance->getCursorY()+15);
        }
        btn->renderCount++;
      }


    }//End the for loop 
}

void Renderer::moveSelectedItem(int DIRECTION)
{
    if(DIRECTION == DIR_DOWN)
    {        
        currentPage->selectedItem++;
        //Set render count of option and button widgets to 0; this will rerender them
        widget* wid;
        for(int i = 0; i < currentPage->widgetCount; i++)
        {
        wid = currentPage->widget_array[i];
        if(wid->widgetType == ITEM_OPTION || wid->widgetType == ITEM_BUTTON)
        {
            wid->renderCount = 0;
        }

        }
        render();
    }
    if(DIRECTION == DIR_UP)
    {
        currentPage->selectedItem--;
        //Set render count of option and button widgets to 0; this will rerender them
        widget* wid;
        for(int i = 0; i < currentPage->widgetCount; i++)
        {
        wid = currentPage->widget_array[i];
        if(wid->widgetType == ITEM_OPTION || wid->widgetType == ITEM_BUTTON)
        {
            wid->renderCount = 0;
        }

        }
        render();
    }
}
