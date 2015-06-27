# X3D - A simplistic 3D rendering program in C

[img/wireframe.png "wireframe"]

This is a simple renderer which uses Athena Widgets for the GUI. It is possible to compile but the interface is not so great compared to more recent GUI frameworks.

You can render in wireframe:

![wireframe](img/x3d-wireframe.png "Wireframe")

Polygonal shading:

![polygonal](img/x3d-polygonal.png "Polygonal")

Gouraud shading:

![gouraud](img/x3d-gouraud.png "Gouraud")

Phong shading:

![phong](img/x3d-phong.png "Phong")


The file format is as follows:

```
<N (# vertices)>
<vertex 0: x y z>
...
<vertex N-1: x y z>
<M (# polygons)>
<polygon 0: #vertices color>
...
<polygon M-1: vertex 0 index (0->N-1)>
```

The order of the vertices in the polygons must be counterclockwise for the shading to work properly.

For example, a simple cube could be represented as follows.

cube.x3d
```
8
0 0 0
0 0 5
0 5 0
0 5 5
5 0 0
5 0 5
5 5 0
5 5 5
6
4 1
0
1
3
2
4 1
0
4
5
1
4 1
2
3
7
6
4 1
6
7
5
4
4 1
6
4
0
2
4 1
5
7
3
1
```



