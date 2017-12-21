- # Introduction    
- # I. Components and APIs
- # II. Public Objects and Classes
1. Object Orientation in FreeType 2
2. The FT_Library class
3. The FT_Face class
4. The FT_Size class
5. The FT_GlyphSlot class
6. The FT_CharMap class
7. Objects Relationship
- # III. Internal Objects and Classes
1. Memory Management
2. Input Streams
3. Modules
4. Summary
- # IV. Module Classes
1. The FT_Module_Class Structure
2. The FT_Module Type
- # V. Interfaces and Services

- # 介绍 
这份文档提供了FreeType 2函数库设计与实现的细节。本文档的目标是让开发人员更好的理解FreeType 2是如何组织的，并让他们扩充、定制和调试它。 
首先，我们先了解这个库的目的，也就是说，为什么会写这个库： 
* 它让客户应用程序方便的访问字体文件，无论字体文件存储在哪里，并且与字体格式无关。
* 方便的提取全局字体数据，这些数据在平常的字体格式中普遍存在。（例如：全局度量标准，字符编码/字符映射表，等等） 
* 方便的提取某个字符的字形数据（度量标准，图像，名字，其他任何东西） 
* 访问字体格式特定的功能（例如，SFNT表，多重控制，OpenType轮廓表） 
Freetype 2的设计也受如下要求很大的影响： 
* 高可移植性。这个库必须可以运行在任何环境中。这个要求引入了一些非常激烈的选择，这些是FreeType2的低级系统界面的一部分。 
* 可扩展性。新特性应该可以在极少改动库基础代码的前提下添加。这个要求引入了非常简单的设计：几乎所有操作都是以模块的形式提供的。 
* 可定制。它应该能够很容易建立一个只包含某个特定项目所需的特性的版本。当你需要集成它到一个嵌入式图形库的字体服务器中时，这是非常重要的。 
* 简洁高效。这个库的主要目标是只有很少cpu和内存资源的嵌入式系统。 
这份文档的其他部分分为几个部分。首先，一些章节介绍了库的基本设计以及Freetype 2内部对象/数据的管理。 
接下来的章节专注于库的定制和与这个话题相关的系统特定的界面，如何写你自己的模块和如何按需裁减库初始化和编译。 

- # 一、组件和API  
FT可以看作是一组组件，每个组件负责一部分任务，它们包括 

* 客户应用程序一般会调用FT高层API，它的功能都在一个组件中，叫做基础层。 
* 根据上下文和环境，基础层会调用一个或多个模块进行工作，大多数情况下，客户应用程序不知道使用那个模块。 
* 基础层还包含一组例程来进行一些共通处理，例如内存分配，列表处理、io流解析、固定点计算等等，这些函数可以被模块随意调用，它们形成了一个底层基础API。 

如下图，表明它们的关系 


另外， 

* 为了一些特殊的构建，基础层的有些部分可以替换掉，也可以看作组件。例如ftsystem组件，负责实现内存管理和输入流访问，还有ftinit，负责库初始化。 
* FT还有一些可选的组件，可以根据客户端应用灵活使用，例如ftglyph组件提供一个简单的API来管理字形映象，而不依赖它们内部表示法。或者是访问特定格式的特性，例如ftmm组件用来访问和管理Type1字体中Multiple Masters数据。 
* 最后，一个模块可以调用其他模块提供的函数，这对在多个字体驱动模块中共享代码和表非常有用，例如truetype和cff模块都使用sfnt模块提供的函数。 

见下图，是对上图的一个细化。 


请注意一些要点： 

* 一个可选的组件可以用在高层API，也可以用在底层API，例如上面的ftglyph； 
* 有些可选组件使用模块特定的接口，而不是基础层的接口，上例中，ftmm直接访问Type1模块来访问数据； 
* 一个可替代的组件能够提供一个高层API的函数，例如，ftinit提供FT_Init_FreeType() 

- # 二、公共对象和类 


1、FT中的面向对象 
虽然FT是使用ANSI C编写，但是采用面向对象的思想，是这个库非常容易扩展，因此，下面有一些代码规约。 

1. 每个对象类型/类都有一个对应的结构类型和一个对应的结构指针类型，后者称为类型/类的句柄类型 

设想我们需要管理FT中一个foo类的对象，可以定义如下 

typedef struct FT_FooRec_* FT_Foo; 

typedef struct FT_FooRec_ 

{ 

// fields for the foo class 

… 

}FT_FooRec; 

依照规约，句柄类型使用简单而有含义的标识符，并以FT_开始，如FT_Foo，而结构体使用相同的名称但是加上Rec后缀。Rec是记录的缩写。每个类类型都有对应的句柄类型； 

2. 类继承通过将基类包装到一个新类中实现，例如，我们定义一个foobar类，从foo类继承，可以实现为 

typedef struct FT_FooBarRec_ * FT_FooBar; 

typedef struct FT_FooBarRec_ 
{ 
FT_FooRec root; //基类 

}FT_FooBarRec; 

可以看到，将一个FT_FooRec放在FT_FooBarRec定义的开始，并约定名为root，可以确保一个foobar对象也是一个foo对象。 

在实际使用中，可以进行类型转换。 

后面 

2、FT_Library类 
这个类型对应一个库的单一实例句柄，没有定义相应的FT_LibraryRec，使客户应用无法访问它的内部属性。 

库对象是所有FT其他对象的父亲，你需要在做任何事情前创建一个新的库实例，销毁它时会自动销毁他所有的孩子，如face和module等。 

通常客户程序应该调用FT_Init_FreeType()来创建新的库对象，准备作其他操作时使用。 

另一个方式是通过调用函数FT_New_Library()来创建一个新的库对象，它在<freetype/ftmodule.h>中定义，这个函数返回一个空的库，没有任何模块注册，你可以通过调用FT_Add_Module()来安装模块。 

调用FT_Init_FreeType()更方便一些，因为他会缺省地注册一些模块。这个方式中，模块列表在构建时动态计算，并依赖ftinit部件的内 容。（见ftinit.c[l73]行，include FT_CONFIG_MODULES_H，其实就是包含ftmodule.h，在ftmodule.h中定义缺省的模块，所以模块数组 ft_default_modules的大小是在编译时动态确定的。） 

3、FT_Face类 
一个外观对象对应单个字体外观，即一个特定风格的特定外观类型，例如Arial和Arial Italic是两个不同的外观。 

一个外观对象通常使用FT_New_Face()来创建，这个函数接受如下参数：一个FT_Library句柄，一个表示字体文件的C文件路径名，一个决定从文件中装载外观的索引（一个文件中可能有不同的外观），和FT_Face句柄的地址，它返回一个错误码。 

FT_Error FT_New_Face( FT_Library library, 
const char* filepathname, 
FT_Long face_index, 
FT_Face* face); 

函数调用成功，返回0，face参数将被设置成一个非NULL值。 

外观对象包含一些用来描述全局字体数据的属性，可以被客户程序直接访问。例如外观中字形的数量、外观家族的名称、风格名称、EM大小等，详见FT_FaceRec定义。 

4、FT_Size类 
每个FT_Face对象都有一个或多个FT_Size对象，一个尺寸对象用来存放指定字符宽度和高度的特定数据，每个新创建的外观对象有一个尺寸，可以通过face->size直接访问。 

尺寸对象的内容可以通过调用FT_Set_Pixel_Sizes()或FT_Set_Char_Size()来改变。 

一个新的尺寸对象可以通过FT_New_Size()创建，通过FT_Done_Size()销毁，一般客户程序无需做这一步，它们通常可以使用每个FT_Face缺省提供的尺寸对象。 

FT_Size 公共属性定义在FT_SizeRec中，但是需要注意的是有些字体驱动定义它们自己的FT_Size的子类，以存储重要的内部数据，在每次字符大小改变时 计算。大多数情况下，它们是尺寸特定的字体hint。例如，TrueType驱动存储CVT表，通过cvt程序执行将结果放入TT_Size结构体中，而 Type1驱动将scaled global metrics放在T1_Size对象中。 

5、FT_GlyphSlot类 
字形槽的目的是提供一个地方，可以很容易地一个个地装入字形映象，而不管它的格式（位图、向量轮廓或其他）。理想的，一旦一个字形槽创建了，任何字形映象 可以装入，无需其他的内存分配。在实际中，只对于特定格式才如此，像TrueType，它显式地提供数据来计算一个槽地最大尺寸。 

另一个字形槽地原因是他用来为指定字形保存格式特定的hint，以及其他为正确装入字形的必要数据。 

基本的FT_GlyphSlotRec结构体只向客户程序展现了字形metics和映象，而真正的实现回包含更多的数据。 

例如，TrueType特定的TT_GlyphSlotRec结构包含附加的属性，存放字形特定的字节码、在hint过程中暂时的轮廓和其他一些东西。 

最后，每个外观对象有一个单一字形槽，可以用face->glyph直接访问。 

6、FT_CharMap类 
FT_CharMap类型用来作为一个字符地图对象的句柄，一个字符图是一种表或字典，用来将字符码从某种编码转换成字体的字形索引。 

单个外观可能包含若干字符图，每个对应一个指定的字符指令系统，例如Unicode、Apple Roman、Windows codepages等等。 

每个FT_CharMap对象包含一个platform和encoding属性，用来标识它对应的字符指令系统。每个字体格式都提供它们自己的FT_CharMapRec的继承类型并实现它们。 

7、对象关系 

