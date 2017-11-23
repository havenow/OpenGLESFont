OpenGL没有绘制文字的功能，OpenGL绘制文字需要自己实现。     
绘制什么样的文字是根据字体库来的。  

# OpenGL实现文字绘制原理  
将所有文字信息写到一张图片上，在将图片做为纹理加载到OpenGL内部，同时将图片上的文字信息做索引，  
当绘制某个文字时，就在字体索引中找文字，在将对应位置的图片贴到窗口表面。  

- #  描述一个字符的信息

```c++
/**
*   描述一个字符的信息
*   在纹理上的位置，偏移距离，以及在哪一个纹理上
*/
class    Character
{
public:
    Character()
    {
        x0          =   0;
        y0          =   0;
        x1          =   0;
        y1          =   0;
        offsetX     =   0;
        offsetY     =   0;
    }
    /**
    *   存储当前字符在纹理上的坐标位置
    *   采用1024大小的纹理，可以存常用的汉字(16像素)
    */
    unsigned int   x0:10;
    unsigned int   y0:10;
    unsigned int   x1:10;
    unsigned int   y1:10;
    //! 字体的偏移x方向
    unsigned int   offsetX:8;
    //! 字体的偏移y方向
    unsigned int   offsetY:8;
    //! 纹理的所有，最大256个
    unsigned int   texIndex:8;//当前字符在哪一个图片上
};

:10  表示占10位
```
- # FreeType  

[FreeType2中文手册](https://www.cnblogs.com/htc-javaMe/archive/2010/12/12/2562529.html)      

包括下面的内容：    
[FreeType Glyph Conventions](https://www.freetype.org/freetype2/docs/glyphs/index.html)   
[FreeType Design](https://www.freetype.org/freetype2/docs/design/index.html)    
[FreeType Tutorial](https://www.freetype.org/freetype2/docs/tutorial/index.html)    

[Examples](https://www.freetype.org/freetype2/docs/tutorial/step3.html)    
For completeness, here again a link to the [example](https://www.freetype.org/freetype2/docs/tutorial/example1.c) used and explained in the [first part of the tutorial](https://www.freetype.org/freetype2/docs/tutorial/step1.html).        

freetype字体库是将文字渲染到矢量或者图片  

