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

\begin{equation}
    \phi^D(x_0) = -2\int_C G(x,x_0)[n(x)\cdot\nabla\phi^D(x)]dl(x) + 2\int^{PV}_C\phi^D(x)[n(x)\cdot\nabla G(x,x_0)]dl(x)
\end{equation}

where $G(x,x_0)$ is the free-space Green’s function of Laplace’s equation in an axisymmetric
domain, and the point $x_0$ lies on the contour of the body $C$.
