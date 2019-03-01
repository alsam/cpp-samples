#include <gtest/gtest.h>
#include "tiny_trimesh.h"

class TinyTriMeshTest : public testing::Test
{
protected:

    TriMesh getTrivial()
    {
        TriMesh mesh;
        std::vector<Point3> vertices{ {0., 0., 0.}, {1., 0., 0.}, {1., 1., 0.}, {0., 1., 0.}, };
        std::vector<Face> faces{ Face({0,1,3}), Face({1,2,3}), };
        mesh.addVertices(std::move(vertices));
        mesh.addFaces(std::move(faces));

        return mesh;
    }

    TriMesh getFourFaces()
    {
        TriMesh mesh;
        std::vector<Point3> vertices{ {0., 0., 0.}, {0.5, 0., 0.}, {1., 0., 0.}, {0.75, 0.5, 0.}, {0.25, 0.5, 0.0}, {0.5, 1., 0.}};
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
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
