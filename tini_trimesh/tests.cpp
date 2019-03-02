// The MIT License (MIT)
//
// Copyright (c) 2019 Alexander Samoilov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

#include <gtest/gtest.h>
#include "tiny_trimesh.h"

class TinyTriMeshTest : public testing::Test
{
protected:

    TriMesh getTrivial()
    {
        /**
         *  3       2
         *   x-----x
         *   |\    |
         *   | \ 1 |
         *   |  \  |
         *   | 0 \ |
         *   |    \|
         *   x-----x
         *  0       1
         **/

        TriMesh mesh;
        std::vector<Point3> vertices{ {0., 0., 0.}, {1., 0., 0.}, {1., 1., 0.}, {0., 1., 0.}, };
        std::vector<Face> faces{ Face({0,1,3}), Face({1,2,3}), };
        mesh.addVertices(std::move(vertices));
        mesh.addFaces(std::move(faces));

        return mesh;
    }

    TriMesh getFourFaces()
    {
        /**
         *              5
         *              x
         *             / \
         *            /   \
         *           /  3  \
         *          /       \
         *      4  x---------x 3
         *        / \       / \
         *       /   \  2  /   \
         *      /  0  \   /  1  \
         *     /       \ /       \
         *    x---------x---------x
         * 0            1          2
         *
         **/

        TriMesh mesh;
        std::vector<Point3> vertices{ {0., 0., 0.}, {0.5, 0., 0.}, {1., 0., 0.},
            {0.75, 0.5, 0.}, {0.25, 0.5, 0.0}, {0.5, 1., 0.}};
        std::vector<Face> faces{ Face({0,1,4}), Face({1,2,3}), Face({1,3,4}), Face({3,5,4}), };
        mesh.addVertices(std::move(vertices));
        mesh.addFaces(std::move(faces));

        return mesh;
    }

};

TEST_F(TinyTriMeshTest, FindOppositeVertexIndexTrivial)
{
    TriMesh mesh = getTrivial();

    ASSERT_EQ(mesh.n_Vertices(), 4);
    ASSERT_EQ(mesh.n_Faces(), 2);
    int opp_index = mesh.findOppositeVertexIndex(0, 1);
    ASSERT_EQ(opp_index, 0);
    opp_index = mesh.findOppositeVertexIndex(1, 0);
    ASSERT_EQ(opp_index, 2);
    opp_index = mesh.findOppositeVertexIndex(1, 1);
    ASSERT_EQ(opp_index, -1);
    opp_index = mesh.findOppositeVertexIndex(1, 3);
    ASSERT_EQ(opp_index, -1);
}

TEST_F(TinyTriMeshTest, FindOppositeVertexFourFaces)
{
    TriMesh mesh = getFourFaces();

    ASSERT_EQ(mesh.n_Vertices(), 6);
    ASSERT_EQ(mesh.n_Faces(), 4);
    int opp_index = mesh.findOppositeVertexIndex(0, 1);
    ASSERT_EQ(opp_index, -1);
    opp_index = mesh.findOppositeVertexIndex(0, 2);
    ASSERT_EQ(opp_index, 0);
    opp_index = mesh.findOppositeVertexIndex(2, 0);
    ASSERT_EQ(opp_index, 3);
    opp_index = mesh.findOppositeVertexIndex(2, 3);
    ASSERT_EQ(opp_index, 1);
    opp_index = mesh.findOppositeVertexIndex(3, 2);
    ASSERT_EQ(opp_index, 5);
    opp_index = mesh.findOppositeVertexIndex(2, 1);
    ASSERT_EQ(opp_index, 4);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
