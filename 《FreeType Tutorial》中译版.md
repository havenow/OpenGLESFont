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
. . . 当初始化库时发生了一个错误 
} 

这个函数负责下面的事情： 

* 它创建一个FreeType 2库的新实例，并且设置句柄library为它。 

* 它装载库中FreeType所知道的每一个模块。除了别的以外，你新建的library对象可以优雅地处理TrueType, Type 1, CID-keyed 和OpenType/CFF字体。 

就像你所看到的，这个函数返回一个错误代码，如同FreeType API的大部分其他函数一样。值为0的错误代码始终意味着操作成功了，否则，返回值指示错误，library设为NULL。 

- # 3．装载一个字体face 

a.从一个字体文件装载 

应用程序通过调用FT_New_Face创建一个新的face对象。一个face对象描述了一个特定的字样和风格。例如，’Times New Roman Regular’和’Times New Roman Italic’对应两个不同的face。 

FT_Library library; /* 库的句柄 */ 
FT_Face face; /* face对象的句柄 */ 

error = FT_Init_FreeType( &library ); 
if ( error ) { ... } 

error = FT_New_Face( library, 
"/usr/share/fonts/truetype/arial.ttf", 
0, 
&face ); 
if ( error == FT_Err_Unknown_File_Format ) 
{ 
... 可以打开和读这个文件，但不支持它的字体格式 
} 
else if ( error ) 
{ 
... 其它的错误码意味着这个字体文件不能打开和读，或者简单的说它损坏了... 
} 

就如你所想到的，FT_NEW_Face打开一个字体文件，然后设法从中提取一个face。它的参数为：

Library 
一个FreeType库实例的句柄，face对象从中建立 

Filepathname 
字体文件路径名（一个标准的C字符串） 

Face_index 
某些字体格式允许把几个字体face嵌入到同一个文件中。 
这个索引指示你想装载的face。 
如果这个值太大，函数将会返回一个错误。Index 0总是正确的。 

Face 
一个指向新建的face对象的指针。 
当失败时其值被置为NULL。 

要知道一个字体文件包含多少个face，只要简单地装载它的第一个face(把face_index设置为0)，face->num_faces的值就指示出了有多少个face嵌入在该字体文件中。 

b.从内存装载 

如果你已经把字体文件装载到内存，你可以简单地使用FT_New_Memory_Face为它新建一个face对象，如下所示： 

FT_Library library; /* 库的句柄 */ 
FT_Face face; /* face对象的句柄 */ 


error = FT_Init_FreeType( &library ); 
if ( error ) { ... } 

error = FT_New_Memory_Face( library, 
buffer, /* 缓存的第一个字节 */ 
size, /* 缓存的大小（以字节表示） */ 
0, /* face索引 */ 
&face ); 
if ( error ) { ... } 

如你所看到的，FT_New_Memory_Face简单地用字体文件缓存的指针和它的大小（以字节计算）代替文件路径。除此之外，它与FT_New_Face的语义一致。 

c.从其他来源装载（压缩文件，网络，等） 

使用文件路径或者预装载文件到内存是简单的，但还不足够。FreeType 2可以支持通过你自己实现的I/O程序来装载文件。 

这是通过FT_Open_Face函数来完成的。FT_Open_Face可以实现使用一个自定义的输入流，选择一个特定的驱动器来打开，乃至当创建该对象时传递外部参数给字体驱动器。我们建议你查阅“FreeType 2参考手册”，学习如何使用它。

- # 4．访问face内容 

一个face对象包含该face的全部全局描述信息。通常的，这些数据可以通过分别查询句柄来直接访问，例如face->num_glyphs。 

FT_FaceRec结构描述包含了可用字段的完整列表。我们在这里详细描述其中的某些： 

Num_glyphs	
这个值给出了该字体face中可用的字形(glyphs)数目。简单来说，一个字形就是一个字符图像。但它不一定符合一个字符代码。 

Flags 
一个32位整数，包含一些用来描述face特性的位标记。例如，标记FT_FACE_FLAG_SCALABLE用来指示该face的字体格式是 可伸缩并且该字形图像可以渲染到任何字符象素尺寸。要了解face标记的更多信息，请阅读“FreeType 2 API 参考”。 

Units_per_EM 
这个字段只对可伸缩格式有效，在其他格式它将会置为0。它指示了EM所覆盖的字体单位的个数。

Num_fixed_size 
这个字段给出了当前face中嵌入的位图的个数。简单来说，一个strike就是某一特定字符象素尺寸下的一系列字形图像。例如，一个字体face可以包含象素尺寸为10、12和14的strike。要注意的是即使是可伸缩的字体格式野可以包含嵌入的位图！ 

Fixed_sizes 
一个指向FT_Bitmap_Size成员组成的数组的指针。每一个FT_Bitmap_Size指示face中的每一个strike的水平和垂直字符象素尺寸。 
注意，通常来说，这不是位图strike的单元尺寸。 
