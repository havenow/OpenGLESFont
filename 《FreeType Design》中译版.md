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
