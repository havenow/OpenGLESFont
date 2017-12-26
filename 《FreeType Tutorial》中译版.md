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
