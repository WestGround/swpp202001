# M1522.002400 Principles and Practices of Software Development

- Instructor: Prof. [Chung-Kil Hur](http://sf.snu.ac.kr/gil.hur)
- TA: [Juneyoung Lee](http://sf.snu.ac.kr/juneyoung.lee/)(@aqjune), [Sung-Hwan Lee](http://sf.snu.ac.kr/sunghwan.lee/)(@Sung-HwanLee)
    + Email address: swpp@sf.snu.ac.kr. 
        * In the case of sending TA an email, specify sender's name and student ID.  
    + Place: Bldg 302 Rm 312-2 

## Announcements 

- Mar. 17th: To build LLVM, please follow [BuildLLVM.md](BuildLLVM.md). If it doesn't work due to insufficient memory / free space, please contact TA to request a laptop for a class. EDIT: please use this form to request a laptop: https://forms.gle/jDV6K4sD61WEZKh99 *until: Mar. 25 Wed*

## Projects

## Assignments

- Mar. 19th: Assignment 1 is announced. See here: https://github.com/aqjune/swpp202001-assn1

If you cannot compile word.cpp, please check whether your `g++` supports c++17. `g++ -version` should print version 7 or upper.

- Mar. 27th: Assignment 2 is announced. Deadline is Apr. 1st, midnight.
See here: https://github.com/snu-sf-class/swpp202001/tree/master/practice/2.assn

NOTE: The goal of assignment 2 is to write a simple function using IR by your own.
Please don't simply create .ll file using `clang -emit-llvm ..` and submit it.
Such .ll file has idioms/variable namings that are generated by compiler optimizations, such as a zero-extended induction variable (variable `i` in `for (i=0; ..)`).

- Apr. 3rd: Assignment 3 is announced. Deadline is Apr. 12nd, midnight.

NOTE: The `Failed to load passes from ...` error at `3.materials/run-passes.sh` was fixed.
Special thanks to Woosung Song.

NOTE 2: N in `polygon` is not larger than 100. input5.txt's answer was incorrect and fixed (thanks to Jeyeon Si). Block names in `unreachable`'s inputs are always lower-case alphabets.

- Apr. 13th: Check the [issue](https://github.com/snu-sf-class/swpp202001/issues/3) for making up project teams.

- Apr. 14th: Assignment 4 is announced. Deadline is Apr. 21th, midnight.
