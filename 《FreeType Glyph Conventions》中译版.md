- # I. Basic Typographic Concepts
1. Font files, format and information
2. Character images and mappings
3. Character and font metrics

- # II. Glyph Outlines
1. Pixels, points and device resolutions
2. Vectorial representation
3. Hinting and bitmap rendering

- # III. Glyph Metrics
1. Baseline, pens and layouts
2. Typographic metrics and bounding boxes
3. Bearings and advances
4. The effects of grid-fitting
5. Text widths and bounding box

- # IV. Kerning
1. Kerning pairs
2. Applying kerning

- # V. Text Processing
1. Writing simple text strings
2. Sub-pixel positioning
3. Simple kerning
4. Right-to-left layouts
5. Vertical layouts

- # VI. FreeType Outlines
1. FreeType outline description and structure
2. Bounding and control box computations
3. Coordinates, scaling, and grid-fitting

- # VII. FreeType Bitmaps
1. Vectorial versus pixel coordinates
2. The FT_Bitmap descriptor
3. Converting outlines into bitmaps and pixmaps

- # 一、基本印刷概念 
1. 字体文件、格式和信息    
字体是一组可以被显示和打印的多样的字符映像，在单个字体中共享一些共有的特性，包括外表、风格、衬线等。按印刷领域的说法，它必须区别一个字体 家族和多种字体外观，后者通常是从同样的模板而来，但是风格不同。例如，Palatino Regular 和 Palatino Italic是两种不同的外观，但是属于同样的家族Palatino。 

单个字体术语根据上下文既可以指家族也可指外观。例如，大多文字处理器的用户用字体指不同的字体家族，然而，大多这些家族根据它们的格式会通过多 个数据文件实现。对于 TrueType来讲，通常是每个外观一个文件（arial.ttf对应Arial Regular外观，ariali.ttf对应Arial Italic外观）这个文件也叫字体，但是实际上只是一个字体外观。 

数字字体是一个可以包含一个和多个字体外观的数据文件，它们每个都包含字符映像、字符度量，以及其他各种有关文本布局和特定字符编码的重要信息。 对有些难用的格式，像Adobe的Type1，一个字体外观由几个文件描述（一个包含字符映象，一个包含字符度量等）。在这里我们忽略这种情况，只考虑一 个外观一个文件的情况，不过在FT2.0中，能够处理多文件字体。 

为了方便说明，一个包含多个外观的字体文件我们叫做字体集合，这种情况不多见，但是多数亚洲字体都是如此，它们会包含两种或多种表现形式的映像，例如横向和纵向布局。 

2. 字符映象和图        
字符映象叫做字形，根据书写、用法和上下文，单个字符能够有多个不同的映象，即多个字形。多个字符也可以有一个字形（例如Roman？？）。字符 和字形之间的关系可能是非常复杂，本文不多述。而且，多数字体格式都使用不太难用的方案存储和访问字形。为了清晰的原因，当说明FT时，保持下面的观念 

* 一个字体文件包含一组字形，每个字形可以存成位图、向量表示或其他结构（更可缩放的格式使用一种数学表示和控制数据/程序的结合方式）。这些字形可以以任意顺序存在字体文件中，通常通过一个简单的字形索引访问。 

* 字体文件包含一个或多个表，叫做字符图，用来为某种字符编码将字符码转换成字形索引，例如ASCII、Unicode、Big5等等。单个字体文件可能包 含多个字符图，例如大多TrueType字体文件都会包含一个Apple特定的字符图和Unicode字符图，使它在Mac和Windows平台都可以使 用。 

3. 字符和字体度量      
每个字符映象都关联多种度量，被用来在渲染文本时，描述如何放置和管理它们。在后面会有详述，它们和字形位置、光标步进和文本布局有关。它们在渲染一个文本串时计算文本流时非常重要。 

每个可缩放的字体格式也包含一些全局的度量，用概念单位表示，描述同一种外观的所有字形的一些特性，例如最大字形外框，字体的上行字符、下行字符和文本高度等。 

虽然这些度量也会存在于一些不可缩放格式，但它们只应用于一组指定字符维度和分辨率，并且通常用象素表示。 
