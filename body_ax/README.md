% `laplace/body_ax` - an excerpt from the book C.Pozrikidis 'A Practical Guide to Boundary-Element Methods'
% with additional notes and commentaries
% composed by Alexander Samoilov

\pagebreak


## Introduction

* This is a code that computes potential flow past an axisymmetric,
compact (singly connected) or toroidal (doubly connected) body with arbitrary
geometry, as illustrated in Figure
* original _Fortran77_ code is here [CFDLAB code for `laplace/body_ax`](http://dehesa.sourceforge.net/CFDLAB/CFDLAB.13.10/07_ptf/body_ax/)

## Mathematical Formulation

The velocity $u$ is decomposed into three parts as
\begin{equation}
    u = u^{\infty} + v + u^D
\end{equation}

where:

* $u^{\infty}$ is the far-field component prevailing far from the body, expressing uniform
(streaming) flow along the x axis of symmetry.
* $v$ is the velocity due to a line vortex ring with specified strength situated in
the interior of the body, generating circulation around the toroidal body. In
the case of flow past a compact body, this component is inconsequential, and
serves only to modify the disturbance velocity.
* $u^D$ is a disturbance velocity expressed by the gradient of the single-valued
harmonic potential $\Phi^D$,

\begin{equation}
    u^D = \nabla\Phi^D
\end{equation}

Requiring the no-penetration boundary condition $u\cdot n = 0$ around the contour of the
body in a meridional plane of constant angle $\varphi$, where $n$ is the unit vector normal to
the body, we derive a boundary condition for the normal derivative of the disturbance
potential,

\begin{equation}
    n\cdot\nabla\phi^D\equiv\frac{\partial\phi^D}{\partial n}=-\left(u^\infty+v\right)\cdot n
\end{equation}

Using the standard boundary-integral formulation, we find that the disturbance potential
satisfies the integral equation of the second kind

\begin{align}
\begin{split}
    \phi^D(x_0) & = -2\int_C G(x,x_0)[n(x)\cdot\nabla\phi^D(x)]dl(x) \\
                & + 2\int^{PV}_C\phi^D(x)[n(x)\cdot\nabla G(x,x_0)]dl(x)
\end{split}
\end{align}

where $G(x,x_0)$ is the free-space Green’s function of Laplace’s equation in an axisymmetric
domain, and the point $x_0$ lies on the contour of the body $C$.
Inserting the boundary condition (3) into (4) and rearranging, we obtain


![\quad  Streamlines of flow past a sphere](sphere_body_ax.pdf)

![\quad  Streamlines of flow past a triangular thorus](thorus_body_ax.pdf)

\begin{align}
\begin{split}
    \phi^D(x_0) & -2\int^{PV}_C\phi^D(x)[n(x)\cdot\nabla  G(x,x_0)]dl(x) \\
                & = 2\int_C (u^\infty + v)\cdot G(x,x_0)n(x)dl(x)
\end{split}
\end{align}

## Numerical Method
The contour of the body in a meridional plane is discretized into a collection of $N$ boundary elements denoted by $E_i, i=1,\ldots,N$.
The boundary elements can be straight segments or circular arcs. The disturbance potential and its normal derivative
are approximated with constant functions the ith element denoted respectively by $\varphi^D_i$ and

\begin{equation}
\left(\frac{\partial{\phi^D}}{\partial{n}}\right)_i=(u^\infty+v^{(i)})\cdot n^{(i)}
\end{equation}

where $n^{(i)}$ and $v^{(i)}$ are the normal vector and velocity induced by the line vortex ring evaluated at the mid-point of the $i$-th element.
Subject to these approximations, the integral equation (5) assumes the discretized
form

\begin{align}
\begin{split}
    \phi^D(x_0) & - 2\sum_{i=1}^N \phi^D_i\int_{E_i}^{PV}n(x)\nabla G\left(x,x_0\right)dl(x) \\
             & = 2\sum_{i=1}^N \left(u^\infty+v^{(i)}\right)\cdot n^{(i)} \int_{E_i}G\left(x,x_0\right)dl(x)
\end{split}
\end{align}

Identifying the point $x_0$ with the mid-point of the $j$-th element denoted by $x_j^M$, where
$j = 1,\ldots,N$, we obtain a system of linear equations for the unknown values $\phi_i^D$,

\begin{align}
\begin{split}
    \phi_j^D & - 2\sum_{i=1}^N \phi^D_i\int_{E_i}^{PV}n(x)\nabla G\left(x,x_j^M\right)dl(x) \\
             & = 2\sum_{i=1}^N \left(u^\infty+v^{(i)}\right)\cdot n^{(i)} \int_{E_i}G\left(x,x_j^M\right)dl(x)
\end{split}
\end{align}

where $j = 1,\ldots,N$.

To illustrate the structure of the linear system more clearly, we collect the two terms
on the left-hand side of (8) and rearrange to obtain

\begin{align}
\begin{split}
    \sum_{i=1}^N \phi^D_i &  \left[\frac{1}{2}\delta_{ij} - \int_{E_i}^{PV}n(x)\nabla G\left(x,x_j^M\right)dl(x)\right] \\
        & = \sum_{i=1}^N \left(u^\infty+v^{(i)}\right)\cdot n^{(i)} \int_{E_i}G\left(x,x_j^M\right)dl(x)
\end{split}
\end{align}

The integrals in equation (9) are computed by numerical methods and the linear
system is solved by Gauss elimination.

## Tangential Velocity
The normal component of the disturbance velocity along the boundary contour is
computed by solving an integral equation. The tangential component is computed
numerically by differentiating the disturbance potential with respect to arc length
along the contour. The tangential component is then added to the normal compo-
nent to yield the total disturbance velocity. The total velocity is computed from the
decomposition expressed by equation (1).

## Computation of the Velocity at a Point in the Flow
To obtain the velocity at a point in the flow, we compute the gradient of the disturbance potential on the right-hand side of
equation (2) using centered differences.
The disturbance potential is evaluated using the discretized integral representation

\begin{align}
\begin{split}
    \phi^D(x_0) & =   \sum_{i=1}^N \left(u^\infty + v^{(i)}\right)\cdot n^{(i)} \int_{E_i}G\left(x,x_0\right)dl(x) \\
                & + \sum_{i=1}^N \phi^D_i\int_{E_i}n(x)\cdot\nabla G\left(x,x_0\right)dl(x)
\end{split}
\end{align}

where the point $x_0$ lies in the domain of the flow.

## Program Depiction

+ Main program: _body\_ax_  
The main program solves an integral equation of the second kind for the boundary
distribution of the disturbance potential, computes the boundary distribution of the
pressure coefficient and the force exerted on the body, and generates streamlines
originating from specified points in the flow.

+ Files to be linked
1. _elm\_arc_  
Discretization of a circular segment into arc elements.
2. _elm\_line_  
Discretization of a straight segment into straight (linear) elements.
3. _body\_ax\_geo_  
Discretization of the boundary geometry.
4. _body\_ax\_sdlp_  
Evaluation of the single- and double-layer harmonic potential over boundary
elements.
5. _body\_ax\_vel_  
Evaluation of the velocity at a specified point in the flow.
6. _ell\_int_  
Evaluation of complete elliptic integrals of the first and second kind.
7. _gauss\_leg_  
Base points and weights for the Gauss-Legendre quadrature.
8. _gel_  
Solution of a linear algebraic system by Gauss elimination.
9. _lgf\_ax\_fs_  
Free-space Green’s function of Laplace’s equation in an axisymmetric domain.
10. _lvr\_ax\_fs_  
Potential and velocity due to a line vortex ring.

+ Input files:
1. _sphere.dat_  
Parameters for flow past a sphere.
2. _torus\_trgl.dat_  
Parameters for flow past a triangular torus.
+ Output files:
1. _body ax.str_  
Streamlines.
2. _body 2d.out_  
Boundary distribution of the disturbance potential and tangential velocity.

