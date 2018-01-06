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


... initialize library ... 
... create face object ... 
... set character size ... 

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
