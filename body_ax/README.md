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
    \phi^D_j & - 2\sum_{i=1}^N \phi^D_i\int_{E_i}^{PV}n(x)\nabla G\left(x,x_j^M\right)dl(x) \\
             & = 2\sum_{i=1}^N \left(u_\infty+v^{(i)}\right)\cdot n^{(i)} \int_{E_i}G\left(x,x_j^M\right)dl(x)
\end{split}
\end{align}

