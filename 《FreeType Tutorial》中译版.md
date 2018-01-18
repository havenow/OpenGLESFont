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

这是“FreeType2 教程”的第一部分。

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

- # 5．设置当前象素尺寸 

对于特定face中与字符大小相关的信息，FreeType 2使用size对象来构造。例如，当字符大小为12点时，使用一个size对象以1/64象素为单位保存某些规格（如ascender或者文字高度）的值。 

当FT_New_Face或它的亲戚被调用，它会自动在face中新建一个size对象，并返回。该size对象可以通过face->size直接访问。 

注意：一个face对象可以同时处理一个或多个size对象，但只有很少程序员需要用到这个功能，因而，我们决定简化该API，（例如，每个face对象只拥有一个size对象）但是这个特性我们仍然通过附加的函数提供。 

当一个新的face对象建立时，对于可伸缩字体格式，size对象默认值为字符大小水平和垂直均为10象素。对于定长字体格式，这个大小是未定义的，这就是你必须在装载一个字形前设置该值的原因。 

使用FT_Set_Char_Size完成该功能。这里有一个例子，它在一个300x300dpi设备上把字符大小设置为16pt。 

error = FT_Set_Char_Size( 
face, /* face对象的句柄 */ 
0, /* 以1/64点为单位的字符宽度 */ 
16*64, /* 以1/64点为单位的字符高度 */ 
300, /* 设备水平分辨率 */ 
300 ); /* 设备垂直分辨率 */ 

注意： 

* 字符宽度和高度以1/64点为单位表示。一个点是一个1/72英寸的物理距离。通常，这不等于一个象素。 

* 设备的水平和垂直分辨率以每英寸点数(dpi)为单位表示。显示设备（如显示器）的常规值为72dpi或96dpi。这个分辨率是用来从字符点数计算字符象素大小的。 

* 字符宽度为0意味着“与字符高度相同”，字符高度为0意味着“与字符宽度相同”。对于其他情况则意味着指定不一样的字符宽度和高度。 

* 水平或垂直分辨率为0时表示使用默认值72dpi。 

* 第一个参数是face对象的句柄，不是size对象的。 

这个函数计算对应字符宽度、高度和设备分辨率的字符象素大小。然而，如果你想自己指定象素大小，你可以简单地调用FT_Set_Pixel_Sizes，就像这样： 

error = FT_Set_Pixel_Sizes( 
face, /* face对象句柄 */ 
0, /* 象素宽度 */ 
16 ); /* 象素高度 */ 

这个例子把字符象素设置为16x16象素。如前所说的，尺寸中的任一个为0意味着“与另一个尺寸值相等”。 

注意这两个函数都返回错误码。通常，错误会发生在尝试对定长字体格式（如FNT或PCF）设置不在face->fixed_size数组中的象素尺寸值。 

- # 6．装载一个字形图像 

a.把一个字符码转换为一个字形索引 

通常，一个应用程序想通过字符码来装载它的字形图像。字符码是一个特定编码中代表该字符的数值。例如，字符码64代表了ASCII编码中的’A’。 

一个face对象包含一个或多个字符表(charmap)，字符表是用来转换字符码到字形索引的。例如，很多TrueType字体包含两个字符 表，一个用来转换Unicode字符码到字形索引，另一个用来转换Apple Roman编码到字形索引。这样的字体既可以用在Windows（使用Unicode）和Macintosh（使用Apple Roman）。同时要注意，一个特定的字符表可能没有覆盖完字体里面的全部字形。 

当新建一个face对象时，它默认选择Unicode字符表。如果字体没包含Unicode字符表，FreeType会尝试在字形名的基础上模拟 一个。注意，如果字形名是不标准的那么模拟的字符表有可能遗漏某些字形。对于某些字体，包括符号字体和旧的亚洲手写字体，Unicode模拟是不可能的。 

我们将在稍后叙述如何寻找face中特定的字符表。现在我们假设face包含至少一个Unicode字符表，并且在调用FT_New_Face时已经被选中。我们使用FT_Get_Char_Index把一个Unicode字符码转换为字形索引，如下所示： 

glyph_index = FT_Get_Char_Index( face, charcode ); 

这个函数会在face里被选中的字符表中查找与给出的字符码对应的字形索引。如果没有字符表被选中，这个函数简单的返回字符码。 

注意，这个函数是FreeType中罕有的不返回错误码的函数中的一个。然而，当一个特定的字符码在face中没有字形图像，函数返回0。按照约定，它对应一个特殊的字形图像――缺失字形，通常会显示一个框或一个空格。 

b.从face中装载一个字形 

一旦你获得了字形索引，你便可以装载对应的字形图像。在不同的字体中字形图像存储为不同的格式。对于固定尺寸字体格式，如FNT或者PCF，每一 个图像都是一个位图。对于可伸缩字体格式，如TrueType或者Type1，使用名为轮廓(outlines)的矢量形状来描述每一个字形。一些字体格 式可能有更特殊的途径来表示字形（如MetaFont――但这个格式不被支持）。幸运的，FreeType2有足够的灵活性，可以通过一个简单的API支 持任何类型的字形格式。 

字形图像存储在一个特别的对象――字形槽(glyph slot)中。就如其名所暗示的，一个字形槽只是一个简单的容器，它一次只能容纳一个字形图像，可以是位图，可以是轮廓，或者其他。每一个face对象都 有一个字形槽对象，可以通过face->glyph来访问。它的字段在FT_GlyphSlotRec结构的文档中解释了。 

通过调用FT_Load_Glyph来装载一个字形图像到字形槽中，如下： 

error = FT_Load_Glyph( 
face, /* face对象的句柄 */ 
glyph_index, /* 字形索引 */ 
load_flags ); /* 装载标志，参考下面 */ 

load_flags的值是位标志集合，是用来指示某些特殊操作的。其默认值是FT_LOAD_DEFAULT即0。 

这个函数会设法从face中装载对应的字形图像： 

* 如果找到一个对应该字形和象素尺寸的位图，那么它将会被装载到字形槽中。嵌入的位图总是比原生的图像格式优先装载。因为我们假定对一个字形，它有更高质量的版本。这可以用FT_LOAD_NO_BITMAP标志来改变。 

* 否则，将装载一个该字形的原生图像，把它伸缩到当前的象素尺寸，并且对应如TrueType和Type1这些格式，也会完成hinted操作。 

字段face->glyph->format描述了字形槽中存储的字形图像的格式。如果它的值不是FT_GLYPH_FORMAT_BITMAP，你可以通过FT_Render_Glyph把它直接转换为一个位图。如下： 

error = FT_Render_Glyph( face->glyph, /* 字形槽 */ 
render_mode ); /* 渲染模式 */ 

render_mode参数是一个位标志集合，用来指示如何渲染字形图像。把它设为FT_RENDER_MODE_NORMAL渲染出一个高质量的抗锯齿(256级灰度)位图。这是默认情况，如果你想生成黑白位图，可以使用FT_RENDER_MODE_MONO标志。 

一旦你生成了一个字形图像的位图，你可以通过glyph->bitmap(一个简单的位图描述符)直接访问，同时用glyph->bitmap_left和glyph->bitmap_top来指定起始位置。 

要注意，bitmap_left是从字形位图当前笔位置到最左边界的水平距离，而bitmap_top是从笔位置（位于基线）到最高边界得垂直距离。他么是正数，指示一个向上的距离。 

下一部分将给出字形槽内容的更多细节，以及如何访问特定的字形信息（包括度量）。 

c.使用其他字符表 

如前面所说的，当一个新face对象创建时，它会寻找一个Unicode字符表并且选择它。当前被选中的字符表可以通过 face->charmap访问。当没有字符表被选中时，该字段为NULL。这种情况在你从一个不含Unicode字符表的字体文件（这种文件现在 非常罕见）创建一个新的FT_Face对象时发生。 

有两种途径可以在FreeType 2中选择不同的字符表。最轻松的途径是你所需的编码已经有对应的枚举定义在FT_FREETYPE_H中，例如FT_ENCODING_BIG5。在这种情况下，你可以简单地调用FT_Select_CharMap，如下： 

error = FT_Select_CharMap( 
face, /* 目标face对象 */ 
FT_ENCODING_BIG5 ); /* 编码 */ 

另一种途径是手动为face解析字符表。这通过face对象的字段num_charmaps和charmaps(注意这是复数)来访问。如你想到 的，前者是face中的字符表的数目，后者是一个嵌入在face中的指向字符表的指针表(a table of pointers to the charmaps)。 

每一个字符表有一些可见的字段，用来更精确地描述它，主要用到的字段是charmap->platform_id和charmap->encoding_id。这两者定义了一个值组合，以更普 
通的形式用来描述该字符表。 

每一个值组合对应一个特定的编码。例如组合(3,1)对应Unicode。组合列表定义在TrueType规范中，但你也可以使用文件FT_TRUETYPE_IDS_H来处理它们，该文件定义了几个有用的常数。 

要选择一个具体的编码，你需要在规范中找到一个对应的值组合，然后在字符表列表中寻找它。别忘记，由于历史的原因，某些编码会对应几个值组合。这里是一些代码： 

FT_CharMap found = 0; 
FT_CharMap charmap; 
int n; 


for ( n = 0; n < face->num_charmaps; n++ ) 
{ 
charmap = face->charmaps[n]; 
if ( charmap->platform_id == my_platform_id && 
charmap->encoding_id == my_encoding_id ) 
{ 
found = charmap; 
break; 
} 
} 

if ( !found ) { ... } 

/* 现在，选择face对象的字符表*/ 
error = FT_Set_CharMap( face, found ); 
if ( error ) { ... } 

一旦某个字符表被选中，无论通过FT_Select_CharMap还是通过FT_Set_CharMap，它都会在后面的FT_Get_Char_Index调用使用。 

d.字形变换 

当字形图像被装载时，可以对该字形图像进行仿射变换。当然，这只适用于可伸缩（矢量）字体格式。 

简单地调用FT_Set_Transform来完成这个工作，如下： 

error = FT_Set_Transform( 
face, /* 目标face对象 */ 
&matrix, /* 指向2x2矩阵的指针 */ 
&delta ); /* 指向2维矢量的指针 */ 

这个函数将对指定的face对象设置变换。它的第二个参数是一个指向FT_Matrix结 
构的指针。该结构描述了一个2x2仿射矩阵。第三个参数是一个指向FT_Vector结构的指针。该结构描述了一个简单的二维矢量。该矢量用来在2x2变换后对字形图像平移。 

注意，矩阵指针可以设置为NULL，在这种情况下将进行恒等变换。矩阵的系数是16.16形式的固定浮点单位。 

矢量指针也可以设置为NULL，在这种情况下将使用(0, 0)的delta。矢量坐标以一个象素的1/64为单位表示（即通常所说的26.6固定浮点格式）。 

注意：变换将适用于使用FT_Load_Glyph装载的全部字形，并且完全独立于任何hinting处理。这意味着你对一个12象素的字形进行2倍放大变换不会得到与24象素字形相同的结果（除非你禁止hints）。 

如果你需要使用非正交变换和最佳hints，你首先必须把你的变换分解为一个伸缩部分和一个旋转/剪切部分。使用伸缩部分来计算一个新的字符象素大小，然后使用旋转/剪切部分来调用FT_Set_Transform。这在本教程的后面部分有详细解释。 

同时要注意，对一个字形位图进行非同一性变换将产生错误。 

- # 7. 简单的文字渲染 

现在我们将给出一个非常简单的例子程序，该例子程序渲染一个8位Latin-1文本字符串，并且假定face包含一个Unicode字符表。 

该程序的思想是建立一个循环，在该循环的每一次迭代中装载一个字形图像，把它转换为一个抗锯齿位图，把它绘制到目标表面(surface)上，然后增加当前笔的位置。 

a.基本代码 

下面的代码完成我们上面提到的简单文本渲染和其他功能。 

FT_GlyphSlot slot = face->glyph; /* 一个小捷径 */ 
int pen_x, pen_y, n; 


... initialize library ..
... create face object ..
... set character size ..

pen_x = 300; 
pen_y = 200; 

for ( n = 0; n < num_chars; n++ ) 
{ 
FT_UInt glyph_index; 


/* 从字符码检索字形索引 */ 
glyph_index = FT_Get_Char_Index( face, text[n] ); 

/* 装载字形图像到字形槽（将会抹掉先前的字形图像） */ 
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT ); 
if ( error ) 
continue; /* 忽略错误 */ 

/* 转换为一个抗锯齿位图 */ 
error = FT_Render_Glyph( face->glyph, ft_render_mode_normal ); 
if ( error ) 
continue; 

/* 现在，绘制到我们的目标表面(surface) */ 
my_draw_bitmap( &slot->bitmap, 
pen_x + slot->bitmap_left, 
pen_y - slot->bitmap_top ); 

/* 增加笔位置 */ 
pen_x += slot->advance.x >> 6; 
pen_y += slot->advance.y >> 6; /* 现在还是没用的 */ 
} 

这个代码需要一些解释： 

* 我们定义了一个名为slot的句柄，它指向face对象的字形槽。（FT_GlyphSlot类型是一个指针）。这是为了便于避免每次都使用face->glyph->XXX。 

* 我们以slot->advance增加笔位置，slot->advance符合字形的步进宽度（也就是通常所说的走格(escapement)）。步进矢量以象素的1/64为单位表示，并且在每一次迭代中删减为整数象素。 

* 函数my_draw_bitmap不是FreeType的一部分，但必须由应用程序提供以用来绘制位图到目标表面。在这个例子中，该函数以一个FT_Bitmap描述符的指针和它的左上角位置为参数。 

* Slot->bitmap_top的值是正数，指字形图像顶点与pen_y的垂直距离。我们假定my_draw_bitmap采用的坐标使用一样的约定（增加Y值对应向下的扫描线）。我们用pen_y减它，而不是加它。 

b.精练的代码 

下面的代码是上面例子程序的精练版本。它使用了FreeType 2中我们还没有介绍的特性和函数，我们将在下面解释： 

FT_GlyphSlot slot = face->glyph; /* 一个小捷径 */ 
FT_UInt glyph_index; 
int pen_x, pen_y, n; 


... initialize library ... 
... create face object ... 
... set character size ... 

pen_x = 300; 
pen_y = 200; 

for ( n = 0; n < num_chars; n++ ) 
{ 
/* 装载字形图像到字形槽（将会抹掉先前的字形图像） */ 
error = FT_Load_Char( face, text[n], FT_LOAD_RENDER ); 
if ( error ) 
continue; /* 忽略错误 */ 

/* 现在，绘制到我们的目标表面(surface) */ 
my_draw_bitmap( &slot->bitmap, 
pen_x + slot->bitmap_left, 
pen_y - slot->bitmap_top ); 

/* 增加笔位置 */ 
pen_x += slot->advance.x >> 6; 
} 

我们简化了代码的长度，但它完成相同的工作： 

* 我们使用函数FT_Loac_Char代替FT_Load_Glyph。如你大概想到的，它相当于先调用GT_Get_Char_Index然后调用FT_Get_Load_Glyph。 

* 我们不使用FT_LOAD_DEFAULT作为装载模式，使用FT_LOAD_RENDER。它指示了字形图像必须立即转换为一个抗锯齿位图。这是一个捷径，可以取消明显的调用FT_Render_Glyph，但功能是相同的。 
注意，你也可以指定通过附加FT_LOAD_MONOCHROME装载标志来获得一个单色位图。 

c.更高级的渲染 

现在，让我们来尝试渲染变换文字（例如通过一个环）。我们可以用FT_Set_Transform来完成。这里是示例代码： 

FT_GlyphSlot slot; 
FT_Matrix matrix; /* 变换矩阵 */ 
FT_UInt glyph_index; 
FT_Vector pen; /* 非变换原点 */ 
int n; 


... initialize library ... 
... create face object ... 
... set character size ... 

slot = face->glyph; /* 一个小捷径 */ 

/* 准备矩阵 */ 
matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L ); 
matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L ); 
matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L ); 
matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L ); 

/* 26.6 笛卡儿空间坐标中笔的位置，以(300, 200)为起始 */ 
pen.x = 300 * 64; 
pen.y = ( my_target_height - 200 ) * 64; 

for ( n = 0; n < num_chars; n++ ) 
{ 
/* 设置变换 */ 
FT_Set_Transform( face, &matrix, &pen ); 

/* 装载字形图像到字形槽（将会抹掉先前的字形图像） */ 
error = FT_Load_Char( face, text[n], FT_LOAD_RENDER ); 
if ( error ) 
continue; /* 忽略错误 */ 

/* 现在，绘制到我们的目标表面（变换位置） */ 
my_draw_bitmap( &slot->bitmap, 
slot->bitmap_left, 
my_target_height - slot->bitmap_top ); 

/* 增加笔位置 */ 
pen.x += slot->advance.x; 
pen.y += slot->advance.y; 
} 

一些说明： 

* 现在我们使用一个FT_Vector类型的矢量来存储笔位置，其坐标以象素的1/64为单位表示，并且倍增。该位置表示在笛卡儿空间。 

* 不同于系统典型的对位图使用的坐标系（其最高的扫描线是坐标0），FreeType中，字形图像的装载、变换和描述总是采用笛卡儿坐标系（这意味着增加Y对应向上的扫描线）。因此当我们定义笔位置和计算位图左上角时必须在两个系统之间转换。 

* 我们对每一个字形设置变换来指示旋转矩阵以及使用一个delta来移动转换后的图像到当前笔位置（在笛卡儿空间，不是位图空间）。结 果，bitmap_left和bitmap_top的值对应目标空间象素中的位图原点。因此，我们在调用my_draw_bitmap时不在它们的值上加 pen.x或pen.y。 

* 步进宽度总会在变换后返回，这就是它可以直接加到当前笔位置的原因。注意，这次它不会四舍五入。 

一个例子完整的源代码可以在这里找到。 

要很注意，虽然这个例子比前面的更复杂，但是变换效果是完全一致的。因此它可以作为一个替换（但更强大）。 

然而该例子有少许缺点，我们将在本教程的下一部分中解释和解决。 

结论 

在这个部分，你已经学习了FreeType2的基础以及渲染旋转文字的充分知识。 

下一部分将深入了解FreeType 2 API更详细的资料，它可以让你直接访问字形度量标准和字形图像，还能让你学习到如何处理缩放、hinting、自居调整，等等。

- # 第二步 －－ 管理字形 

介绍 

这是“FreeType2 教程”的第二部分。它将教会你如何： 

* 检索字形度量 
* 容易地管理字形图像 
* 检索全局度量（包括字距调整） 
* 渲染一个简单的字符串（采用字距调整） 
* 渲染一个居中的字符串（采用字距调整） 
* 渲染一个变换的字符串（采用居中） 
* 在需要时以预设字体单位的格式获取度量，以及把它们缩放到设备空间 

- # 1.字形度量 

从字形度量这个名字可以想到，字形度量是关联每一个字形的确定距离，以此描述如何使用该距离来排版文本。 

通常一个字形有两个度量集：用来排版水平文本排列的字形（拉丁文、西里尔文、阿拉伯文、希伯来文等等）和用来排版垂直文本排列的字形（中文、日文、韩文等等）。 

要注意的是只有很少的字体格式提供了垂直度量。你可以使用宏FT_HAS_VERTICAL测试某个给出的face对象是否包含垂直度量，当结果为真时表示包含。 

个别的字形度量可以先装载字形到face的字形槽，然后通过face->glyph->metrics结构访问，其类型为FT_Glyph_Metrics。我们将在下面详细讨论它，现在，我们只关注该结构包含如下的字段： 

Width 
这是字形图像的边框的宽度。它与排列方向无关。 

Height 
这是字形图像的边框的高度。它与排列方向无关。 

horiBearingX 
用于水平文本排列，这是从当前光标位置到字形图像最左边的边界的水平距离。 

horiBearingY 
用于水平文本排列，这是从当前光标位置（位于基线）到字形图像最上边的边界的水平距离。 

horiAdvance 
用于水平文本排列，当字形作为字符串的一部分被绘制时，这用来增加笔位置的水平距离。 

vertBearingX 
用于垂直文本排列，这是从当前光标位置到字形图像最左边的边框的垂直距离。 

vertBearingY 
用于垂直文本排列，这是从当前光标位置（位于基线）到字形图像最上边的边框的垂直距离。 

vertAdvance 
用于垂直文本排列，当字形作为字符串的一部分被绘制时，这用来增加笔位置的垂直距离。 

注意：因为不是所有的字体都包含垂直度量，当FT_HAS_VERTICAL为假时，vertBearingX，vertBearingY和vertAdvance的值是不可靠的。 

下面的图形更清楚地图解了度量。第一个图解了水平度量，其基线为水平轴： 
freetype2 开发手册 - Jesse Rei - Jesse Rei 的博客 步入... 

对于垂直文本排列，基线是垂直的，与垂直轴一致： 
freetype2 开发手册 - Jesse Rei - Jesse Rei 的博客 步入... 

Face->glyph->metrics中的度量通常以26.6象素格式（例如1/64象素）表示，除非你在调用FT_Load_Glyph或FT_Load_Char时使用了FT_LOAD_NO_SCALE标志，这样的话度量会用原始字体单位表示。 

字形槽(glyph slot)对象也有一些其他有趣的字段可以减轻开发者的工作。你可以通过face->glyph->xxx访问它们，其中xxx是下面字段之一： 

Advance 
这个字段是一个FT_Vector，保存字形的变换推进。当你通过FT_Set_Transform使用变换时，这是很有用的，这在第一部分的循 环文本例子中已经展示过了。与那不同，这个值是默认的(metrics.horiAdvance,0)，除非你在装载字形图像时指定 FT_LOAD_VERTICAL，那么它将会为(0,metrics.vertAdvance)。 

linearHoriAdvance 
这个字段包含字形水平推进宽度的线性刻度值。实际上，字形槽返回的metrics.horiAdvance值通常四舍五入为整数象素坐标（例如， 它是64的倍数），字体驱动器用它装载字形图像。linearHoriAdvance是一个16.16固定浮点数，提供了以1/65536象素为单位的原 始字形推进宽度的值。它可以用来完成伪设备无关文字排版。 

linearVertAdvance 
这与linearHoriAdvance类似，但它用于字形的垂直推进高度。只有当字体face包含垂直度量时这个值才是可靠的。 

- # 2.管理字形图像 

转载到字形槽得字形图像可以转换到一幅位图中，这可以在装载时使用FT_LOAD_RENDER标志或者调用FT_Render_Glyph函数实现。每一次你装载一个新的字形图像到字形槽，前面装载的将会从字形槽中抹去。 

但是，你可能需要从字形槽中提取这个图像，以用来在你的应用程序中缓存它，或者进行附加的变换，或者在转换到位图前测量它。 

FreeType 2 API有一个特殊的扩展能够以一种灵活和普通的方式处理字形图像。要使用它，你首先需要包含FT_GLYPH_H头文件，如下： 

#include FT_GLYPH_H 

现在我们将解释如何使用这个文件定义的这个函数。 

a.提取字形图像 

你可以很简单地提取一个字形图像。这里有一向代码向你展示如何去做： 

FT_Glyph glyph; /* 字形图像的句柄 */ 

... 
error = FT_Load_Glyph( face, glyph_index, FT_LOAD_NORMAL ); 
if ( error ) { ... } 

error = FT_Get_Glyph( face->glyph, &glyph ); 
if ( error ) { ... } 

如你所看到的，我们： 

* 创建一个类型为FT_Glyph，名为glyph的变量。这是一个字形图像的句柄（即指针）。 

* 装载字形图像（通常情况下）到face的字形槽中。我们不使用FT_LOAD_RENDER因为我们想抓取一个可缩放的字形图像，以便后面对其进行变换。 

* 通过调用FT_Get_Glyph，把字形图像从字形槽复制到新的FT_Glyph对象glyph中。这个函数返回一个错误码并且设置glyph。 

要非常留意，被取出的字形跟字形槽中的原始字形的格式是一样的。例如，如果我们从TrueType字体文件中装载一个字形，字形图像将是可伸缩的矢量轮廓。 

如果你想知道字形是如何模型和存储的，你可以访问flyph->format字段。一个新的字形对象可以通过调用FT_Done_Glyph来销毁。 

字形对象正好包含一个字形图像和一个2D矢量，2D矢量以16.16固定浮点坐标的形式表示字形的推进。后者可以直接通过glyph->advance访问。 

注意，不同于其他TrueType对象，库不保存全部分配了的字形对象的列表。这意味着你必须自己销毁它们，而不是依靠FT_Done_FreeType完成全部的清除。 

b.变换和复制字形图像 

如果字形图像是可伸缩的（例如，如果glyph->format不等于FT_GLYPH_FORMAT_BITMAP），那么就可以随时通过调用FT_Glyph_Transform来变换该图像。 

你也可以通过FT_Glyph_Copy复制一个字形图像。这里是一些例子代码： 

FT_Glyph glyph, glyph2; 
FT_Matrix matrix; 
FT_Vector delta; 

... 装载字形图像到 `glyph' ... 

/* 复制 glyph 到 glyph2 */ 

error = FT_Glyph_Copy( glyph, &glyph2 ); 
if ( error ) { ... 无法复制（内存不足） ... } 

/* 平移 `glyph' */ 

delta.x = -100 * 64; /* 坐标是 26.6 象素格式的 */ 
delta.y = 50 * 64; 

FT_Glyph_Transform( glyph, 0, &delta ); 

/* 变换 glyph2 （水平剪切） */ 

matrix.xx = 0x10000L; 
matrix.xy = 0.12 * 0x10000L; 
matrix.yx = 0; 
matrix.yy = 0x10000L; 

FT_Glyph_Transform( glyph2, &matrix, 0 ); 

注意，2x2矩阵变换总是适用于字形的16.16推进矢量，所以你不需要重修计算它。 

c.测量字形图像 

你也可以通过FT_Glyph_Get_CBox函数检索任意字形图像（无论是可伸缩或者不可伸缩的）的控制（约束）框，如下： 

FT_BBox bbox; 

... 
FT_Glyph_Get_CBox( glyph, bbox_mode, &bbox ); 

坐标是跟字形的原点(0, 0)相关的，使用y向上的约定。这个函数取一个特殊的参数：bbox_mode来指出如何表示框坐标。 

如果字形装载时使用了FT_LOAD_NO_SCALE标志，bbox_mode必须设置为FT_GLYPH_BBOX_UNSCALED，以此 来获得以26.6象素格式为单位表示的不可缩放字体。值FT_GLYPH_BBOX_SUBPIXELS是这个常量的另一个名字。 

要注意，框(box)的最大坐标是唯一的，这意味着你总是可以以整数或26.6象素的形式计算字形图像的宽度和高度，公式如下： 

width = bbox.xMax - bbox.xMin; 
height = bbox.yMax - bbox.yMin; 

同时要注意，对于26.6坐标，如果FT_GLYPH_BBOX_GRIDFIT被用作为bbox_mode，坐标也将网格对齐，符合如下公式： 

bbox.xMin = FLOOR( bbox.xMin ) 
bbox.yMin = FLOOR( bbox.yMin ) 
bbox.xMax = CEILING( bbox.xMax ) 
bbox.yMax = CEILING( bbox.yMax ) 

要把bbox以整数象素坐标的形式表示，把bbox_mode设置为FT_GLYPH_BBOX_TRUNCATE。 

最后，要把约束框以网格对齐象素坐标的形式表示，把bbox_mode设置为FT_GLYPH_BBOX_PIXELS。 

d.转换字形图像为位图 

当你已经把字形对象缓存或者变换后，你可能需要转换它到一个位图。这可以通过FT_Glyph_To_Bitmap函数简单得实现。它负责转换任何字形对象到位图，如下： 

FT_Vector origin; 
origin.x = 32;	/* 26.6格式的1/2象素 */ 
origin.y = 0; 
error = FT_Glyph_To_Bitmap( 
&glyph, 
render_mode, 
&origin, 
1 );	/* 销毁原始图像 == true */ 

一些注解： 

* 第一个参数是源字形句柄的地址。当这个函数被调用时，它读取该参数来访问源字形对象。调用结束后，这个句柄将指向一个新的包含渲染后的位图的字形对象。 

* 第二个参数时一个标准渲染模式，用来指定我们想要哪种位图。它取FT_RENDER_MODE_DEFAULT时表示8位颜色深度的抗锯齿位图；它取FT_RENDER_MODE_MONO时表示1位颜色深度的黑白位图。 

* 第三个参数是二维矢量的指针。该二维矢量是在转换前用来平移源字形图像的。要注意，函数调用后源图像将被平移回它的原始位置（这样便不会有变化）。如果你在渲染前不需要平移源字形，设置这个指针为0。 

* 最后一个参数是一个布尔值，用来指示该函数是否要销毁源字形对象。如果为false，源字形对象不会被销毁，即使它的句柄丢失了（客户应用程序需要自己保留句柄）。 

如果没返回错误，新的字形对象总是包含一个位图。并且你必须把它的句柄进行强制类型转换，转换为FT_BitmapGlyph类型，以此访问它的内容。这个类型是FT_Glyph的一种“子类”，它包含下面的附加字段（看FT_BitmapGlyphRec）： 

Left 
类似于字形槽的bitmap_left字段。这是字形原点(0,0)到字形位图最左边象素的水平距离。它以整数象素的形式表示。 
Top 
类似于字形槽的bitmap_top字段。它是字形原点(0,0)到字形位图最高象素之间的垂直距离（更精确来说，到位图上面的象素）。这个距离以整数象素的形式表示，并且y轴向上为正。 
Bitmap 
这是一个字形对象的位图描述符，就像字形槽的bitmap字段。 

- # 3.全局字形度量 

不同于字形度量，全局度量是用来描述整个字体face的距离和轮廓的。他们可以用26.6象素格式或者可缩放格式的“字体单位”来表示。 

a.预设全局度量 

对于可缩放格式，全部全局度量都是以字体单位的格式表示的，这可以用来在稍后依照本教程本部分的最后一章描述的规则来缩放到设备空间。你可以通过FT_Face句柄的字段直接访问它们。 

然而，你需要在使用它们前检查字体face的格式是否可缩放。你可以使用宏FT_IS_SCALEABLE来实现，当该字体是可缩放时它返回正。 

如果是这样，你就可以访问全局预设度量了，如下： 

units_per_EM 
这是字体face的EM正方形的大小。它是可缩放格式用来缩放预设坐标到设备象素的，我们在这部分的最后一章叙述它。通常这个值为2048（对于 TrueType）或者1000（对于Type 1），但是其他值也是可能的。对于固定尺寸格式，如FNT/FON/PCF/BDF，它的值为1。 

global_bbox 
全局约束框被定义为最大矩形，该矩形可以包围字体face的所有字形。它只为水平排版而定义。 

ascender 
Ascender是从水平基线到字体face最高“字符”的坐标之间的垂直距离。不幸地，不同的字体格式对ascender的定义是不同的。对于 某些来说，它代表了全部大写拉丁字符（重音符合除外）的上沿值(ascent)；对于其他，它代表了最高的重音符号的上沿值(ascent)；最后，其他 格式把它定义为跟global_bbox.yMax相同。 

descender 
Descender是从水平基线到字体face最低“字符”的坐标之间的垂直距离。不幸地，不同的字体格式对descender的定义是不同的。 对于某些来说，它代表了全部大写拉丁字符（重音符合除外）的下沿值(descent)；对于其他，它代表了最高的重音符号的下沿值(descent)；最 后，其他格式把它定义为跟global_bbox.yMin相同。这个字段的值是负数。 

text_height 
这个字段是在使用这个字体书写文本时用来计算默认的行距的（例如，基线到基线之间的距离）。注意，通常它都比ascender和descent的绝对值之和还要大。另外，不保证使用这个距离后面就没有字形高于或低于基线。 

max_advance_width 
这个字段指出了字体中所有字形得最大的水平光标推进宽度。它可以用来快速计算字符串得最大推进宽度。它不等于最大字形图像宽度！ 

max_advance_height 
跟max_advance_width一样，但是用在垂直文本排版。它只在字体提供垂直字形度量时才可用。 

underline_position 
当显示或者渲染下划线文本时，这个值等于下划线到基线的垂直距离。当下划线低于基线时这个值为负数。 

underline_thickness 
当显示或者渲染下划线文本时，这个值等于下划线的垂直宽度。 

现在注意，很不幸的，由于字体格式多种多样，ascender和descender的值是不可靠的。 

b.伸缩的全局度量 

每一个size对象同时包含了上面描述的某些全局度量的伸缩版本。它们可以通过face->size->metrics结构直接访问。 

注意这些值等于预设全局变量的伸缩版本，但没有做舍入或网格对齐。它们也完全独立于任何hinting处理。换句话说，不要依靠它们来获取象素级别的精确度量。它们以26.6象素格式表示。 

ascender 
原始预设ascender的伸缩版本。 

descender 
原始预设ascender的伸缩版本。 

height 
原始预设文本高度(text_height)的伸缩版本。这可能是这个结构中你真正会用到的字段。 

max_advance 
原始预设最大推进的伸缩版本。 

注意，face->size->metrics结构还包含其他字段，用来伸缩预设坐标到设备空间。它们会在最后一章描述。 

c.字距调整 

字距调整是调整字符串中两个并排的字形图像位置的过程，它可以改善文本的整体外观。基本上，这意味着当‘A’的跟着‘V’时，它们之间的间距可以稍微减少，以此避免额外的“对角线空白”。 

注意，理论上字距调整适用于水平和垂直方向的两个字形，但是，除了非常极端的情况外，几乎在所有情况下，它只会发生在水平方向。 

不是所有的字体格式包含字距调整信息。有时候它们依赖于一个附加的文件来保存不同的字形度量，包括字距调整，但该文件不包含字形图像。一个显著的 例子就是Type1格式。它的字形图像保存在一个扩展名为.pfa或.pfb的文件中，字距调整度量存放在一个附加的扩展名为.afm或.pfm的文件 中。 

FreeType 2提供了FT_Attach_File和FT_Attach_Stream API来让你处理这种情况。两个函数都是用来装载附加的度量到一个face对象中，它通过从附加的特定格式文件中读取字距调整度量来实现。例如，你可以象 下面那样打开一个Type1字体： 

error = FT_New_Face( library, "/usr/shared/fonts/cour.pfb", 
0, &face ); 
if ( error ) { ... } 

error = FT_Attach_File( face, "/usr/shared/fonts/cour.afm" ); 
if ( error ) 
{ ... 没能读取字距调整和附加的度量 ... } 

注意，FT_Attach_Stream跟FT_Attach_File是类似的，不同的是它不是以C字符串指定附加文件，而是以一个FT_Stream句柄。另外，读取一个度量文件不是强制性的。 

最后，文件附加API是非常通用的，可以用来从指定的face中装载不同类型的附加信息。附加内容的种类完全是因字体格式而异的。 

FreeType 2允许你通过FT_Get_Kerning函数获取两个字形的字距调整信息，该函数界面如下： 

FT_Vector kerning; 

... 
error = FT_Get_Kerning( face, /* face对象的句柄 */ 
left, /* 左边字形索引 */ 
right, /* 右边字形索引 */ 
kerning_mode, /* 字距调整模式 */ 
&kerning ); /* 目标矢量 */ 

正如你所见到的，这个函数的参数有一个face对象的句柄、字距调整值所要求的左边和右边字形索引，以及一个称为字距调整模式的整数，和目标矢量的指针。目标矢量返回适合的距离值。 

字距调整模式跟前一章描述的bbox模式(bbox mode)是很类似的。这是一个枚举值，指示了目标矢量如何表示字距调整距离。 

默认值是FT_KERNING_DEFAULT，其数值为0。它指示字距调整距离以26.6网格对齐象素（这意味着该值是64的倍数）的形式表示。对于可伸缩格式，这意味着返回值是把预设字距调整距离先伸缩，然后舍入。 

值FT_KERNING_UNFITTED指示了字距调整距离以26.6非对齐象素（也就是，那不符合整数坐标）的形式表示。返回值是把预设字距调整伸缩，但不舍入。 

最后，值FT_KERNING_UNSCALED是用来返回预设字距调整距离，它以字体单位的格式表示。你可以在稍后用本部分的最后一章描述的算式把它拉伸到设备空间。 

注意，“左”和“右”位置是指字符串字形的可视顺序。这对双向或由右到左的文本来说是很重要的。 

- # 4.简单的文本渲染：字距调整+居中 

为了显示我们刚刚学到的知识，现在我们将示范如何修改第一部分给出的代码以渲染一个字符串，并且增强它，使它支持字距调整和延迟渲染。 

a.字距调整支持 

要是我们只考虑处理从左到右的文字，如拉丁文，那在我们的代码上添加字距调整是很容易办到的。我们只要获取两个字形之间的字距调整距离，然后适当地改变笔位置。代码如下： 

FT_GlyphSlot slot = face->glyph; /* 一个小捷径 */ 
FT_UInt glyph_index; 
FT_Bool use_kerning; 
FT_UInt previous; 
int pen_x, pen_y, n; 

... 初始化库 ... 
... 创建face对象 ... 
... 设置字符尺寸 ... 

pen_x = 300; 
pen_y = 200; 

use_kerning = FT_HAS_KERNING( face ); 
previous = 0; 

for ( n = 0; n < num_chars; n++ ) 
{ 
/* 把字符码转换为字形索引 */ 
glyph_index = FT_Get_Char_Index( face, text[n] ); 

/* 获取字距调整距离，并且移动笔位置 */ 
if ( use_kerning && previous && glyph_index ) 
{ 
FT_Vector delta; 

FT_Get_Kerning( face, previous, glyph_index, 
ft_kerning_mode_default, &delta ); 

pen_x += delta.x >> 6; 
} 

/* 装载字形图像到字形槽（擦除之前的字形图像） */ 
Error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER); 
if ( error ) 
continue; /* 忽略错误 */ 

/* 现在绘制到我们的目标表面(surface) */ 
my_draw_bitmap( &slot->bitmap, 
pen_x + slot->bitmap_left, 
pen_y - slot->bitmap_top ); 

/* 增加笔位置 */ 
pen_x += slot->advance.x >> 6; 

/* 记录当前字形索引 */ 
previous = glyph_index; 
} 

若干注解： 

* 因为字距调整是由字形索引决定的，我们需要显式转换我们的字符代码到字形索引，然后调用FT_Load_Glyph而不是FT_Load_Char。 

* 我们使用一个名为use_kerning的变量，它的值为宏FT_HAS_KERNING的结果。当我们知道字体face不含有字距调整信息，不调用FT_Get_kerning程序将执行得更快。 

* 我们在绘制一个新字形前移动笔位置。 

* 我们以值0初始化变量previous，这表示“字形缺失(missing glyph)”（在Postscript中，这用.notdef表示）。该字形也没有字距调整距离。 

* 我们不检查FT_Get_kerning返回得错误码。这是因为这个函数在错误发生时总是把delta置为(0,0)。 

b.居中 

我们的代码开始变得有趣了，但对普通应用来说仍然有点太简单了。例如，笔的位置在我们渲染前就决定了。通常，你要在计算文本的最终位置（居中，等）前布局它和测量它，或者执行自动换行。 

现在让我们把文字渲染函数分解为两个截然不同但连续的两部分：第一部分将在基线上定位每一个字形图像，第二部分将渲染字形。我们将看到，这有很多好处。 

我们先保存每一个独立的字形图像，以及它们在基线上面的位置。这可以通过如下的代码完成： 

FT_GlyphSlot slot = face->glyph; /* 一个小捷径 */ 
FT_UInt glyph_index; 
FT_Bool use_kerning; 
FT_UInt previous; 
int pen_x, pen_y, n; 

FT_Glyph glyphs[MAX_GLYPHS]; /* 字形图像 */ 
FT_Vector pos [MAX_GLYPHS]; /* 字形位置 */ 
FT_UInt num_glyphs; 

... 初始化库 ... 
... 创建face对象 ... 
... 设置字符尺寸 ... 

pen_x = 0; /* 以 (0,0) 开始 */ 
pen_y = 0; 

num_glyphs = 0; 
use_kerning = FT_HAS_KERNING( face ); 
previous = 0; 

for ( n = 0; n < num_chars; n++ ) 
{ 
/* 把字符码转换为字形索引 */ 
glyph_index = FT_Get_Char_Index( face, text[n] ); 

/* 获取字距调整距离，并且移动笔位置 */ 
if ( use_kerning && previous && glyph_index ) 
{ 
FT_Vector delta; 

FT_Get_Kerning( face, previous, glyph_index, 
FT_KERNING_DEFAULT, &delta ); 

pen_x += delta.x >> 6; 
} 

/* 保存当前笔位置 */ 
pos[num_glyphs].x = pen_x; 
pos[num_glyphs].y = pen_y; 

/* 装载字形图像到字形槽，不渲染它 */ 
error=FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT); 
if ( error ) 
continue; /* 忽略错误，跳到下一个字形 */ 

/* 提取字形图像并把它保存在我们的表中 */ 
error = FT_Get_Glyph( face->glyph, &glyphs[num_glyphs] ); 
if ( error ) 
continue; /* 忽略错误，跳到下一个字形 */ 

/* 增加笔位置 */ 
pen_x += slot->advance.x >> 6; 

/* 记录当前字形索引 */ 
previous = glyph_index; 

/* 增加字形数量 */ 
num_glyphs++; 
} 

相对于我们之前的代码，这有轻微的变化：我们从字形槽中提取每一个字形图像，保存每一个字形图像和它对应的位置在我们的表中。 

注意pen_x包含字符串的整体前移值。现在我们可以用一个很简单的函数计算字符串的边界框(bounding box)，如下： 

void compute_string_bbox( FT_BBox *abbox ) 
{ 
FT_BBox bbox; 

/* 初始化字符串bbox为“空”值 */ 
bbox.xMin = bbox.yMin = 32000; 
bbox.xMax = bbox.yMax = -32000; 

/* 对于每一个字形图像，计算它的边界框，平移它，并且增加字符串bbox */ 
for ( n = 0; n < num_glyphs; n++ ) 
{ 
FT_BBox glyph_bbox; 

FT_Glyph_Get_CBox( glyphs[n], ft_glyph_bbox_pixels, 
&glyph_bbox ); 

glyph_bbox.xMin += pos[n].x; 
glyph_bbox.xMax += pos[n].x; 
glyph_bbox.yMin += pos[n].y; 
glyph_bbox.yMax += pos[n].y; 

if ( glyph_bbox.xMin < bbox.xMin ) 
bbox.xMin = glyph_bbox.xMin; 

if ( glyph_bbox.yMin < bbox.yMin ) 
bbox.yMin = glyph_bbox.yMin; 

if ( glyph_bbox.xMax > bbox.xMax ) 
bbox.xMax = glyph_bbox.xMax; 

if ( glyph_bbox.yMax > bbox.yMax ) 
bbox.yMax = glyph_bbox.yMax; 
} 

/* 检查我们是否真的增加了字符串bbox */ 
if ( bbox.xMin > bbox.xMax ) 
{ 
bbox.xMin = 0; 
bbox.yMin = 0; 
bbox.xMax = 0; 
bbox.yMax = 0; 
} 

/* 返回字符串bbox */ 
*abbox = bbox; 
} 

最终得到的边界框尺寸以整数象素的格式表示，并且可以随后在渲染字符串前用来计算最终的笔位置，如下： 

/* 计算整数象素表示的字符串尺度 */ 
string_width = string_bbox.xMax - string_bbox.xMin; 
string_height = string_bbox.yMax - string_bbox.yMin; 

/* 计算以26.6笛卡儿象素表示的笔起始位置*/ 
start_x = ( ( my_target_width - string_width ) / 2 ) * 64; 
start_y = ( ( my_target_height - string_height ) / 2 ) * 64; 

for ( n = 0; n < num_glyphs; n++ ) 
{ 
FT_Glyph image; 
FT_Vector pen; 

image = glyphs[n]; 

pen.x = start_x + pos[n].x; 
pen.y = start_y + pos[n].y; 

error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 
&pen, 0 ); 
if ( !error ) 
{ 
FT_BitmapGlyph bit = (FT_BitmapGlyph)image; 

my_draw_bitmap( bit->bitmap, 
bit->left, 
my_target_height - bit->top ); 

FT_Done_Glyph( image ); 
} 
} 

一些说明： 

* 笔位置以笛卡儿空间（例如，y向上）的形式表示。 

* 我们调用FT_Glyph_To_Bitmap时destroy参数设置为0(false)，这是为了避免破坏原始字形图像。在执行该调用后，新的字形位图通过image访问，并且它的类型转变为FT_BitmapGlyph。 

* 当调用FT_Glyph_To_Bitmap时，我们使用了平移。这可以确保位图字形对象的左区域和上区域已经被设置为笛卡儿空间中的正确的象素坐标。 

* 当然，在渲染前我们仍然需要把象素坐标从笛卡儿空间转换到设备空间。因此在调用my_draw_bitmap前要先计算my_target_height – bitmap->top。 

相同的循环可以用来把字符串渲染到我们的显示面(surface)任意位置，而不需要每一次都重新装载我们的字形图像。我们也可以决定实现自动换行或者只是绘制。 

- # 5.高级文本渲染：变换 + 居中 + 字距调整 

现在我们将修改我们的代码，以便可以容易地变换已渲染的字符串，例如旋转它。我们将以实行少许小改进开始： 

a.打包然后平移字形 

我们先把与一个字形图像相关的信息打包到一个结构体，而不是并行的数组。因此我们定义下面的结构体类型： 

typedef struct TGlyph_ 
{ 
FT_UInt index; /* 字形索引 */ 
FT_Vector pos; /* 基线上面的字形原点 */ 
FT_Glyph image; /* 字形图像 */ 
} TGlyph, *PGlyph; 

我们在装载每一个字形图像过程中，在把它装载它在基线所在位置后便直接平移它。我们将看到，这有若干好处。我们的字形序列装载其因而变成： 

FT_GlyphSlot slot = face->glyph; /* 一个小捷径 */ 
FT_UInt glyph_index; 
FT_Bool use_kerning; 
FT_UInt previous; 
int pen_x, pen_y, n; 

TGlyph glyphs[MAX_GLYPHS]; /* 字形表 */ 
PGlyph glyph; /* 表中的当前字形*/ 
FT_UInt num_glyphs; 

... 初始化库 ... 
... 创建face对象 ... 
... 设置字符尺寸 ... 

pen_x = 0; /* 以 (0,0) 开始 */ 
pen_y = 0; 

num_glyphs = 0; 
use_kerning = FT_HAS_KERNING( face ); 
previous = 0; 

glyph = glyphs; 
for ( n = 0; n < num_chars; n++ ) 
{ 
glyph->index = FT_Get_Char_Index( face, text[n] ); 

if ( use_kerning && previous && glyph->index ) 
{ 
FT_Vector delta; 

FT_Get_Kerning( face, previous, glyph->index, 
FT_KERNING_MODE_DEFAULT, &delta ); 

pen_x += delta.x >> 6; 
} 

/* 保存当前笔位置 */ 
glyph->pos.x = pen_x; 
glyph->pos.y = pen_y; 

error = FT_Load_Glyph(face,glyph_index,FT_LOAD_DEFAULT); 
if ( error ) continue; 

error = FT_Get_Glyph( face->glyph, &glyph->image ); 
if ( error ) continue; 

/* 现在平移字形图像 */ 
FT_Glyph_Transform( glyph->image, 0, &glyph->pos ); 

pen_x += slot->advance.x >> 6; 
previous = glyph->index; 

/* 增加字形的数量 */ 
glyph++; 
} 

/* 计算已装载的字形的数量 */ 
num_glyphs = glyph - glyphs; 

注意，这个时候平移字形有若干好处。第一是当我们计算字符串的边界框时不需要平移字形bbox。代码将会变成这样： 

void compute_string_bbox( FT_BBox *abbox ) 
{ 
FT_BBox bbox; 

bbox.xMin = bbox.yMin = 32000; 
bbox.xMax = bbox.yMax = -32000; 

for ( n = 0; n < num_glyphs; n++ ) 
{ 
FT_BBox glyph_bbox; 

FT_Glyph_Get_CBox( glyphs[n], &glyph_bbox ); 

if (glyph_bbox.xMin < bbox.xMin) 
bbox.xMin = glyph_bbox.xMin; 

if (glyph_bbox.yMin < bbox.yMin) 
bbox.yMin = glyph_bbox.yMin; 

if (glyph_bbox.xMax > bbox.xMax) 
bbox.xMax = glyph_bbox.xMax; 

if (glyph_bbox.yMax > bbox.yMax) 
bbox.yMax = glyph_bbox.yMax; 
} 

if ( bbox.xMin > bbox.xMax ) 
{ 
bbox.xMin = 0; 
bbox.yMin = 0; 
bbox.xMax = 0; 
bbox.yMax = 0; 
} 

*abbox = bbox; 
} 

更详细描述：compute_string_bbox函数现在可以计算一个已转换的字形字符串的边界框。例如，我们可以做如下的事情： 

FT_BBox bbox; 
FT_Matrix matrix; 
FT_Vector delta; 

... 装载字形序列 ... 
... 设置 "matrix" 和 "delta" ... 

/* 变换字形 */ 
for ( n = 0; n < num_glyphs; n++ ) 
FT_Glyph_Transform( glyphs[n].image, &matrix, &delta ); 

/* 计算已变换字形的边界框 */ 
compute_string_bbox( &bbox ); 

b.渲染一个已变换的字形序列 

无论如何，如果我们想重用字形来以不同的角度或变换方式绘制字符串，直接变换序列中的字形都不是一个好主意。更好的方法是在字形被渲染前执行放射变换，如下面的代码所示： 

FT_Vector start; 
FT_Matrix transform; 

/* 获取原始字形序列的 bbox */ 
compute_string_bbox( &string_bbox ); 

/* 计算整数象素表示的字符串尺度 */ 
string_width = (string_bbox.xMax - string_bbox.xMin) / 64; 
string_height = (string_bbox.yMax - string_bbox.yMin) / 64; 

/* 设置26.6笛卡儿空间表示的笔起始位置 */ 
start.x = ( ( my_target_width - string_width ) / 2 ) * 64; 
start.y = ( ( my_target_height - string_height ) / 2 ) * 64; 

/* 设置变换（旋转） */ 
matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L ); 
matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L ); 
matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L ); 
matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L ); 

for ( n = 0; n < num_glyphs; n++ ) 
{ 
FT_Glyph image; 
FT_Vector pen; 
FT_BBox bbox; 

/* 创建原始字形的副本 */ 
error = FT_Glyph_Copy( glyphs[n].image, &image ); 
if ( error ) continue; 

/* 变换副本（这将平移它到正确的位置） */ 
FT_Glyph_Transform( image, &matrix, &start ); 

/* 检查边界框；如果已变换的字形图像不在*/ 
/* 我们的目标表面中，我们可以避免渲染它 */ 
FT_Glyph_Get_CBox( image, ft_glyph_bbox_pixels, &bbox ); 
if ( bbox.xMax <= 0 || bbox.xMin >= my_target_width || 
bbox.yMax <= 0 || bbox.yMin >= my_target_height ) 
continue; 

/* 把字形图像转换为位图（销毁字形的副本！） */ 
error = FT_Glyph_To_Bitmap( 
&image, 
FT_RENDER_MODE_NORMAL, 
0, /* 没有附加的平移*/ 
1 ); /* 销毁 "image" 指向的副本 */ 
if ( !error ) 
{ 
FT_BitmapGlyph bit = (FT_BitmapGlyph)image; 

my_draw_bitmap( bitmap->bitmap, 
bitmap->left, 
my_target_height - bitmap->top ); 
FT_Done_Glyph( image ); 
} 
} 

这份代码相对于原始版本有少许改变： 

* 我们没改变原始的字形图像，而是变换该字形图像的拷贝。 

* 我们执行“剪取”操作以处理渲染和绘制的字形不在我们的目标表面(surface)的情况。 

* 当调用FT_Glyhp_To_Bitmap时，我们总是销毁字形图像的拷贝，这是为了销毁已变换的图像。注意，即使当这个函数返回错误码，该图像依然会被销毁（这就是为什么FT_Done_Glyph只在复合语句中被调用的原因）。 

* 平移字形序列到起始笔位置集成到FT_Glyph_Transform函数，而不是FT_Glyph_To_Bitmap函数。 

可以多次调用这个函数以渲染字符串到不同角度的，或者甚至改变计算start的方法以移动它到另外的地方。 

这份代码是FreeType 2示范程序ftstring.c的基础。它可以被简单地扩展，在第一部发完成高级文本布局或自动换行，而第二部分不需改变。 

无论如何，要注意通常的实现会使用一个字形缓冲以减少内存消耗。据个例子，让我们假定我们的字符串是“FreeType”。我们将在我们的表中保存字母‘e’的三个相同的字形图像，这不是最佳的（特别是当你遇到更长的字符串或整个页面时）。 

- # 6.以预设字体单位的格式访问度量，并且伸缩它们 

可伸缩的字体格式通常会为字体face中的每一个字形保存一份矢量图像，该矢量图像称为轮廓。每一个轮廓都定义在一个抽象的网格中，该网格被称为 预设空间(design space)，其坐标以名义上(nominal)的字体单位(font unit)表示。当装载一个字形图像时，字体驱动器通常会依照FT_Size对象所指定的当前字符象素尺寸把轮廓伸缩到设备空间。字体驱动器也能修改伸缩 过的轮廓以大大地改善它在基于象素的表面(surface)中显示的效果。修改动作通常称为hinting或网格对齐。 

这一章描述了如何把预设坐标伸缩到设备空间，以及如何读取字形轮廓和如何获取以预设字体单位格式表示的度量。这对许多事情来说都是重要的： 

* 真正的所见即所得文字排版 

* 为了字体转换或者分析的目的而访问字体内容 

a.伸缩距离到设备空间 

我们使用一个简单的伸缩变换把预设坐标伸缩到设备空间。变换系数借助字符象素尺寸来计算： 

Device_x = design_x * x_scale 
Device_y = design_y * y_scale 

X_scale = pixel_size_x / EM_size 
Y_scale = pixel_size_y / EM_size 

这里，值EM_Size是因字体而异的，并且对应预设空间的一个抽象矩形（称为EM）的大小。字体设计者使用该矩形创建字形图像。EM_Size 以字体单元的形式表示。对于可伸缩字体格式，可以通过face->unix_per_EM直接访问。你应该使用FT_IS_SCALABLE宏检查 某个字体face是否包含可伸缩字形图像，当包含时该宏返回true。 

当你调用函数FT_Set_Pixel_Sizes，你便指定了pixel_size_x和pixel_size_y的值。FreeType库将会立即使用该值计算x_scale和y_scale的值。 

当你调用函数FT_Set_Char_Size，你便以物理点的形式指定了字符尺寸。FreeType库将会使用该值和设备的解析度来计算字符象素尺寸和相应的比例因子。 

注意，在调用上面提及的两个函数后，你可以通过访问face->size->metrices结构的字段得到字符象素尺寸和比例因子的值。这些字段是： 

X_ppem 
这个字段代表了“每一个EM的x方向象素”，这是以整数象素表示EM矩形的水平尺寸，也是字符水平象素尺寸，即上面例子所称的pixel_size_x。 

y_ppem 
这个字段代表了“每一个EM的y方向象素”，这是以整数象素表示EM矩形的垂直尺寸，也是字符垂直象素尺寸，即上面例子所称的pixel_size_y。 

X_scale 
这是一个16.16固定浮点比例，用来把水平距离从预设空间直接伸缩到1/64设备象素。 

y_scale 
这是一个16.16固定浮点比例，用来把垂直距离从预设空间直接伸缩到1/64设备象素。 

你可以借助FT_MulFix函数直接伸缩一个以26.6象素格式表示的距离，如下所示： 

/* 把预设距离转换到1/64象素 */ 
pixels_x=FT_MulFix(design_x,face->size->metrics.x_scale); 
pixels_y=FT_MulFix(design_y,face->size->metrics.y_scale); 

当然，你也可以使用双精度浮点数更精确地伸缩该值： 

FT_Size_Metrics* metrics = &face->size->metrics; /* 捷径 */ 
double pixels_x, pixels_y; 
double em_size, x_scale, y_scale; 

/* 计算浮点比例因子 */ 
em_size = 1.0 * face->units_per_EM; 
x_scale = metrics->x_ppem / em_size; 
y_scale = metrics->y_ppem / em_size; 

/* 把预设距离转换为浮点象素 */ 
pixels_x = design_x * x_scale; 
pixels_y = design_y * y_scale; 

b.访问预设度量（字形的和全局的） 

你可以以字体单位的格式访问字形度量，只要在调用FT_Load_Glyph或FT_Load_Char时简单地指定FT_LOAD_NO_SCALE位标志便可以了。度量返回在face->glyph_metrics，并且全部都以字体单位的格式表示。 

你可以使用FT_KERNING_MODE_UNSCALED模式访问未伸缩的字距调整数据。 

最后，FT_Face句柄的字段包含少数几个全局度量，我们已经在本部分的第三章叙述过了。 

结论 

这是FreeType 2教程第二部分的结尾。现在你可以访问字形度量，管理字形图像，以及更巧妙地渲染文字（字距调整，测量，变换和缓冲）。 

现在你有了足够的知识能够以FreeType2为基础构建一个相当好的文字服务，而且要是你愿意，你可以在这里止步了。 

下一部分将涉及FreeType2的内部（例如模块，矢量轮廓，设备驱动器，渲染器），以及少数的字体格式特有的问题（主要是，如何访问某些TrueType或Type 1表）。【这部分还没有编写】
