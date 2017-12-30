- # I. Simple Glyph Loading
1. Header Files
2. Library Initialization
3. Loading a Font Face
4. Accessing the Face Data
5. Setting the Current Pixel Size
6. Loading a Glyph Image
7. Simple Text Rendering

- # II. Managing Glyphs
1. Glyph Metrics
2. Managing Glyph Images
3. Global Glyph Metrics
4. Simple Text Rendering: Kerning and Centering
5. Advanced Text Rendering: Transformation and Centering and Kerning
6. Accessing Metrics in Design Font Units, and Scaling Them
Conclusion

- # 第一步 －－ 简易的字形装载 

介绍 

这是“FreeType2 教程”的第一部分。它将教会你如何： 

* 初始化库 
* 通过创建一个新的 face 对象来打开一个字体文件 
* 以点或者象素的形式选择一个字符大小 
* 装载一个字形(glyph)图像，并把它转换为位图 
* 渲染一个简单的字符串 
* 容易地渲染一个旋转的字符串 

- # 1.头文件 

下面的内容是编译一个使用了FreeType2库的应用程序所需要的指令。请谨慎阅读，自最近一次版本更新后我们已经更改了少许东西。 

1.FreeType2 include 目录 

你必须把FreeType2头文件的目录添加到编译包含(include)目录中。 

注意，现在在Unix系统，你可以运行freetype-config脚本加上--cflags选项来获得正确的编译标记。这个脚本也可以用来检查安装在你系统中的库的版本，以及需要的库和连接标记。 

2. 包含名为ft2build.h的文件 

Ft2build.h包含了接下来要#include的公共FreeType2头文件的宏声明。 

3. 包含主要的FreeType2 API头文件 

你要使用FT_FREETYPE_H宏来完成这个工作，就像下面这样： 

#include <ft2build.h> 
#include FT_FREETYPE_H 

FT_FREETYPE_H是在ftheader.h中定义的一个特别的宏。Ftheader.h包含了一些安装所特定的宏，这些宏指名了FreeType2 API的其他公共头文件。 

你可以阅读“FreeType 2 API参考”的这个部分来获得头文件的完整列表。 

＃include语句中宏的用法是服从ANSI的。这有几个原因： 

* 这可以避免一些令人痛苦的与FreeType 1.x公共头文件的冲突。 

* 宏名字不受限于DOS的8.3文件命名限制。象FT_MULTIPLE_MASTERS_H或FT_SFNT_NAMES_H这样的名字比真实的文件名ftmm.h和fsnames.h更具可读性并且更容易理解。 

* 它允许特别的安装技巧，我们不在这里讨论它。 

注意：从FreeType 2.1.6开始，旧式的头文件包含模式将不会再被支持。这意味着现在如果你做了象下面那样的事情，你将得到一个错误： 

#include <freetype/freetype.h>    
#include <freetype/ftglyph.h>    


- # 2. 初始化库 

简单地创建一个FT_Library类型的变量，例如library，然后象下面那样调用函数FT_Init_FreeType： 

#include <ft2build.h> 
#include FT_FREETYPE_H 

FT_LIBRARY library; 



Error = FT_Init_FreeType ( &library ); 
If ( error ) 
{ 
. . . 当初始化库时发生了一个错误 . . . 
} 

这个函数负责下面的事情： 

* 它创建一个FreeType 2库的新实例，并且设置句柄library为它。 

* 它装载库中FreeType所知道的每一个模块。除了别的以外，你新建的library对象可以优雅地处理TrueType, Type 1, CID-keyed 和OpenType/CFF字体。 

就像你所看到的，这个函数返回一个错误代码，如同FreeType API的大部分其他函数一样。值为0的错误代码始终意味着操作成功了，否则，返回值指示错误，library设为NULL。 
