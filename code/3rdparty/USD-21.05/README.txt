GETTING STARTED WITH USD BINARIES FROM USD.NVIDIA.COM

This document describes how to run some basic sanity checks upon downloading and extracting USD binaries from usd.nvidia.com

The steps below assume that the contents of the archives hosted on usd.nvidia.com have been extracted to a single folder, that we reference as a shell variable USDROOT.

[ Windows ]

(Built on Windows 10, VS2017)


== Python 3.6 ==
(Tested on python 3.6.7)

- Add %USDROOT%\bin and %USDROOT%\lib to the front of %PATH%
- Add %USDROOT%\lib\python to the front of %PYTHONPATH%
- Install PySide2 and PyOpenGL to python, e.g., python -m pip install PySide2 PyOpenGL
- usdcat.cmd %USDROOT%\share\usd\tutorials\convertingLayerFormats\Sphere.usd should return

#usda 1.0

def Sphere "sphere"
{
}


- usdview.cmd %USDROOT%\share\usd\tutorials\traversingStage\HelloWorld.usda should bring up usdview with a simple Sphere.



[ Linux ]

(Built on a Centos 7 docker image, tested on Ubuntu 18.04)


== Python 3.6 ==
(Tested on python 3.6.9)

- Add $USDROOT/bin to the front of $PATH
- Add $USDROOT/lib/python to the front of $PYTHONPATH
- Install pip, GLEW and QT5, e.g., sudo apt install -y python3-pip glew-utils qt5-default
- Install PySide2 and PyOpenGL to python, e.g., python3 -m pip install PySide2 PyOpenGL
- usdcat $USDROOT/share/usd/tutorials/convertingLayerFormats/Sphere.usd should return

#usda 1.0

def Sphere "sphere"
{
}


- usdview $USDROOT/share/usd/tutorials/traversingStage/HelloWorld.usda should bring up usdview with a simple Sphere.

