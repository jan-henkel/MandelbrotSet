INTRODUCTION
============

MandelbrotSet is an image generator for Mandelbrot- and Julia-type sets
written in C++ and Qt. The author is Jan Henkel.

MATHEMATICAL BACKGROUND
=======================

Given any mathematical function f, the 'orbit' of a point p under f is 
the sequence p, f(p), f(f(p)), ... . For instance, let f(x)=x^2+0.1, 
then the orbit of 2 under f is 2,2^2+0.1=4.1, 4.1^2+0.1=16.91, ... .

The mandelbrot set is defined as the set of complex numbers c for which 
the orbit of 0 under f(z)=z^2+c remains bounded. The orbit in this case 
is the sequence 0, c, c^2+c, (c^2+c)^2+c, ... . 
It can be shown that this orbit diverges to infinity if |z|^2 exceeds 4.
Julia sets on the other hand are defined for fixed c as the set of 
initial values z for which the orbit under f(z)=z^2+c remains bounded.

In practice, one checks whether after at most a given maximum number
of iterations (repeated applications of f) |z|^2 exceeds a certain 
limit (e.g. 4).

Each pixel of the generated image corresponds to a complex number, 
either c in the case of Mandelbrot-type sets or the initial value of z
in the case of Julia-sets.
To yield more aesthetic imagery than a 2 colored image consisting of 
the inside and outside of a Mandelbrot- or Julia-type set, one colors
pixels depending on the number of iterations it took to meet the escape
criterion (i.e. for |z|^2 to exceed the limit).
Other aspects of the result of the iteration may also be used to 
influence coloring.

USAGE
=====

Navigation
----------

Press and hold the right mouse button on the render area, then drag and
let go to displace the section of the set which is being rendered.
Left click and drag to select an area to zoom in on. Alternatively zoom
in on the current center either by pressing the '+' key or by using the
mouse wheel. Use the '-' key or mouse wheel to zoom out.

To render the Julia set corresponding to a point, right click on it and
click on 'Explore Julia set' in the context-menu. You can also center
the view on a point by right clicking on it and clicking on
'Center on this point' in the context-menu.

Another way of navigating the sets is to manually enter coordinates to
center on as well as a scale factor. The real and imaginary parts of c
can also be set manually in the case of Julia-type sets.

To apply manual changes and re-render the image, click on the 'Apply'
button in the bottom right of the window.

Color palettes
--------------

To pick a new color palette either click 'Select color palette' and
select an image file from the dialog or drag-drop an image file onto the
color palette preview area (where the current palette is displayed).
Click the 'Apply' button to re-render the image.

Configurations
--------------

Different configurations (which encompass the formula, the area of the
set to be rendered and the color scheme) can be loaded by selecting them
from the combo box in the top right corner.
An image is promptly rendered, using the selected settings. If, after
navigating a set, you wish to restore the initial settings of the
configuration last selected, click 'Restore configuration'.
If you wish to save the current settings, enter a name for your
configuration in the combo box on the top right, then click 
'Save configuration'. If you pick an already existing name, the
configuration in question will be overwritten, with one exception:
Default configurations can not be changed or overwritten.
To view the rendered result of the current settings before saving, you
may need to click 'Apply'.

To delete a configuration no longer needed, simply select it from the
combo box in the top right, then click 'Delete configuration'. Again
this will have no effect on default configurations.

Custom formulas
---------------

To explore sets other than the standard Mandelbrot set, you can change
the formula f(z) which is being iterated. The math expression parser
recognizes the following:

Floating point constants:
E.g. 1.5, 1e-10, 2 and so on

Variables:
z 		variable which is assigned the result f(z) in each
		iteration, e.g. z^2+c for the default Mandelbrot set
				
c		corresponds to pixels on the render area for Mandelbrot-
		type sets, fixed and user defined for Julia sets
				
i		the imaginary unit

Operators:
+,-,*,/ as well as ^ , which is exponentiation with integer exponents
(for general exponentiation use the slower pow(a,b))

Functions:
sin,cos,tan	trigonometric functions

exp, log	the exponential function and natural logarithm

Re, Im 		yield the real and imaginary part of their arguments
				
sqrt		the square root

pow		general exponentiation, takes the 2 arguments, the first
		argument being the base, the second the exponent

Example: pow(z,2)+sqrt(c)+log(Im(z)^2+1)*i

You may also want to adjust the escape limit and number of iterations.

To apply your modifications, click the 'Apply' button.

Custom color schemes
--------------------

Besides picking arbitrary images as color palettes to create interesting
imagery, you can also change the way the color palette is being used.
This is done by setting formulas which map the outcome of the iteration
to X- and Y-coordinates of the image. 

Functions and operators can be used as described in the previous section.

The variables available are:
s,t		real and imaginary parts of z when the iteration loop
		is done
				
u,v		real and imaginary parts of c - in the case of Julia type
		sets, the initial value of z is taken instead of c

n		the number of iterations when either the escape criterion
		is met (i.e. the limit is exceeded) or the maximum number
		of iterations is reached
				
m		the maximum number of iterations, as set by the user

w,h		the width and height (in pixels) of the color palette

If the width or height of the palette are exceeded, the remainder after
division by the width or height respectively is used. Results <0 or
results which are NaN (not a number, a result of e.g. division by zero)
are discarded and 0 is used instead.

Furthermore the user can decide whether the first row and/or column of
are to be used whenever the max. number of iterations is reached (meaning
the point was detected as an element of the set).
If both options are selected, this ensures that the area of the set is
always plain-colored.

The default mapping is n/m*(w-1) for the X-component and 0 for the
Y-component.
Another mapping is (n+1-log(log(s^2+t^2)/log(4))/log(2))/m*(w-1) for X,
0 for Y. This is used to reduce aliasing in the case of the standard
Mandelbrot set.

To apply your custom color scheme, again you have to click the 'Apply'
button.