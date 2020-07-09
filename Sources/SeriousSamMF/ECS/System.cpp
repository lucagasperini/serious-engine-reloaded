#include "System.h"

extern POINT cursor;
extern SDL_Event* event;

void ScalePositionSystem::init()
{
    ULONG width = main_dp->GetWidth();
    ULONG height = main_dp->GetHeight();
    FOREACHINDYNAMICCONTAINER(*pEntities, SEEntity, entity)
    {
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        if (!position)
            continue;

        FLOAT scaleX = (FLOAT)width / (FLOAT)old_width;
        FLOAT scaleY = (FLOAT)height / (FLOAT)old_height;

        position->x = (FLOAT)position->x * scaleX;
        position->y = (FLOAT)position->y * scaleY;
        position->w = (FLOAT)position->w * scaleX;
        position->h = (FLOAT)position->h * scaleY;
    }
    old_width = width;
    old_height = height;
}

void AlignPositionSystem::init()
{
    ULONG center_x = main_dp->GetWidth() /2;
    ULONG center_y = main_dp->GetHeight() /2;
    FOREACHINDYNAMICCONTAINER(*pEntities, SEEntity, entity)
    {
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        SEAlignComponent* align = dynamic_cast<SEAlignComponent *>((SEEntity*)entity);
        if (!position || !align)
            continue;

        switch(align->align_x)
        {
            case -2:
                position->x = 0;
                break;
            case -1:
                position->x = center_x - position->w;
                break;
            case 0:
                position->x = center_x - position->w / 2;
                break;
            case 1:
                position->x = center_x;
                break;
            case 2:
                position->x = center_x * 2 - position->w;
                break;
        }
        switch(align->align_y)
        {
            case -2:
                position->y = 0;
                break;
            case -1:
                position->y = center_y - position->h;
                break;
            case 0:
                position->y = center_y - position->h / 2;
                break;
            case 1:
                position->y = center_y;
                break;
            case 2:
                position->y = center_y * 2 - position->y;
                break;
        }
    }
}

void TextureRenderSystem::init() 
{
    FOREACHINDYNAMICCONTAINER(*pEntities, SEEntity, entity)
    {
        SETextureComponent *texture = dynamic_cast<SETextureComponent *>((SEEntity*)entity);
        SEPositionComponent *position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        if (!texture || !position)
            continue;

        texture->tex.SetData_t(texture->fntex);
    }
}

void TextureRenderSystem::update()
{
    FOREACHINDYNAMICCONTAINER(*pEntities, SEEntity, entity)
    {
        SETextureComponent* texture = dynamic_cast<SETextureComponent *>((SEEntity*)entity);
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        if (!texture || !position)
            continue;

        main_dp->PutTexture(&texture->tex, PIXaabbox2D(
                            PIX2D(position->x, position->y), 
                            PIX2D(position->x + position->w, position->y + position->h)));
    }
}

void ButtonRenderSystem::update()
{
    FOREACHINDYNAMICCONTAINER(*pEntities, SEEntity, entity)
    {
        SEFontComponent* font = dynamic_cast<SEFontComponent *>((SEEntity*)entity);
        SETextComponent* text = dynamic_cast<SETextComponent *>((SEEntity*)entity);
        SEPositionComponent* position = dynamic_cast<SEPositionComponent *>((SEEntity*)entity);
        SEButtonComponent* button = dynamic_cast<SEButtonComponent *>((SEEntity*)entity);
        if (!font || !position || !text)
            continue;
        
        
        if( position->x < cursor.x && 
            position->y < cursor.y && 
            position->x + position->w > cursor.x && 
            position->y + position->h > cursor.y){
            focus = TRUE;
        } else {
            focus = FALSE;
        }

        if( event->type == SDL_MOUSEBUTTONDOWN && 
            event->button.button == SDL_BUTTON_LEFT && 
            button->function != NULL &&
            focus) 
        {
            button->function();
        }
        
        
        main_dp->SetFont(&font->fontdata);
        main_dp->SetTextScaling(1.0f);
        main_dp->SetTextAspect(1.0f);
        main_dp->SetTextMode(font->textmode);

        PIXaabbox2D box(PIX2D(position->x, position->y), PIX2D(position->x + position->w, position->y + position->h));
        COLOR col;

        if(focus) {
            col = font->color2;
        } else {
            col = font->color;
        }

        if (font->align == -1)
            main_dp->PutText(text->str, box.Min()(1), box.Min()(2), col);
        else if (font->align == +1)
            main_dp->PutTextR(text->str, box.Max()(1), box.Min()(2), col);
        else
            main_dp->PutTextC(text->str, box.Center()(1), box.Min()(2), col);
    }
}

void BorderRenderSystem::update()
{
    FOREACHINDYNAMICCONTAINER(*pEntities,SEEntity,entity) {
        SEPositionComponent* position = dynamic_cast< SEPositionComponent* >((SEEntity*)entity);
        if(!position) 
            continue;

        main_dp->DrawBorder(position->x, position->y, position->w, position->h, C_BLACK|0xff);
    }
}