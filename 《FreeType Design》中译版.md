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
