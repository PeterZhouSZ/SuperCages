#include "meshReconstructor.h"

#include "math/leastSquareSolver.h"

MeshReconstructor::MeshReconstructor()
{

}

MeshReconstructor::MeshReconstructor(const Trimesh & t)
{
   init(t);
}

void MeshReconstructor::init(const Trimesh & t)
{
   _t = t;

   int numberOfVertices = t.getNumVertices();

   _differentialCoordinatesX.resize(numberOfVertices+1);
   _differentialCoordinatesY.resize(numberOfVertices+1);
   _differentialCoordinatesZ.resize(numberOfVertices+1);

   for(int i=0; i<numberOfVertices+1; ++i)
   {
      _differentialCoordinatesX[i] = 0.0;
      _differentialCoordinatesY[i] = 0.0;
      _differentialCoordinatesZ[i] = 0.0;
   }

   _entriesOfL.reserve((numberOfVertices+1)*3);

   _L.resize(numberOfVertices+1, numberOfVertices);

   computeTopologicalLaplacianTriplets(_t,_entriesOfL);
   computeUniformDifferentialCoordinates(
                                  _t,
                                  _differentialCoordinatesX,
                                  _differentialCoordinatesY,
                                  _differentialCoordinatesZ);
}

void MeshReconstructor::computeTopologicalLaplacianTriplets
   (const Trimesh & t,
    std::vector<Eigen::Triplet<double>> & entries
   )
{
   //https://eigen.tuxfamily.org/dox/classEigen_1_1SparseMatrix.html#acc35051d698e3973f1de5b9b78dbe345

   int numberOfVertices = t.getNumVertices();

   for(int i = 0; i < numberOfVertices; ++i)
   {
      std::vector<int> v2v = t.v2v(i);
      int numberOfNeighb = v2v.size();
      entries.push_back(Entry(i, i, v2v.size()));
      for(int j=0; j<numberOfNeighb; ++j)
      {
         int neigh = v2v[j];
         entries.push_back(Entry(i, neigh, -1));
      }
   }
}

Trimesh MeshReconstructor::reconstruct()
{
   int index = 0;

   int numVertices = _t.getNumVertices();

   cg3::Vec3d v = _t.getVertex(index);

   _entriesOfL.push_back(Entry(numVertices, index, 1));
   _differentialCoordinatesX[numVertices] = v.x();
   _differentialCoordinatesY[numVertices] = v.y();
   _differentialCoordinatesZ[numVertices] = v.z();

   _L.setFromTriplets(_entriesOfL.begin(), _entriesOfL.end());
   _L.makeCompressed();

   LeastSquareSolver s(_L);

   Eigen::VectorXd X;
   Eigen::VectorXd Y;
   Eigen::VectorXd Z;

   s.solve(_differentialCoordinatesX, X);
   s.solve(_differentialCoordinatesY, Y);
   s.solve(_differentialCoordinatesZ, Z);

   std::vector<double> recVertices(numVertices*3);

   for(int i=0; i<numVertices; ++i)
   {
      recVertices[i*3+0] = X[i];
      recVertices[i*3+1] = Y[i];
      recVertices[i*3+2] = Z[i];
   }

   std::vector<int> tris = _t.getTrianglesVector();
   Trimesh t(recVertices,tris);
   return t;
}

void MeshReconstructor::computeUniformDifferentialCoordinates
   (const Trimesh & t,
    Eigen::VectorXd & differentialCoordinatesX,
    Eigen::VectorXd & differentialCoordinatesY,
    Eigen::VectorXd & differentialCoordinatesZ
   )
{
   //Implementa controllo per cui diffCoords.size > t.numVertices
   int numberOfVertices = t.getNumVertices();
   for(int i = 0; i < numberOfVertices; ++i)
   {
      cg3::Vec3d vi = t.getVertex(i);
      std::vector<int> v2v = t.v2v(i);
      int di = v2v.size();

      for(int j=0; j<di; ++j)
      {
         int jPtr = v2v[j];
         cg3::Vec3d vj = t.getVertex(jPtr);

         cg3::Vec3d delta = (vi-vj);
         differentialCoordinatesX[i] += delta.x();
         differentialCoordinatesY[i] += delta.y();
         differentialCoordinatesZ[i] += delta.z();
      }

      differentialCoordinatesX[i] /= di;
      differentialCoordinatesY[i] /= di;
      differentialCoordinatesZ[i] /= di;
   }

}


void MeshReconstructor::computeCotagentDifferentialCoordinates
   (const Trimesh & t,
    Eigen::VectorXd & differentialCoordinatesX,
    Eigen::VectorXd & differentialCoordinatesY,
    Eigen::VectorXd & differentialCoordinatesZ
   )
{
   //Da finire
   /*
   std::vector<double> trisAreas(t.getNumTriangles());
   computeTrisAreas(t,trisAreas);

   int numberOfVertices = t.getNumVertices();
   for(int i = 0; i < numberOfVertices; ++i)
   {

      cg3::Vec3d vi = t.getVertex(i);
      std::vector<int> v2v = t.v2v(i);
      int numberOfNeighb = v2v.size();

      for(int j=0; j<numberOfNeighb; ++j)
      {
         int jPtr = v2v[j];
         cg3::Vec3d vj = t.getVertex(jPtr);

         //NON VA BENE. PRENDI OGNI EDGE E I DUE VERTICI OPPOSTI COME NEXT E PREV
         cg3::Vec3d vPrev = t.getVertex((jPtr+numberOfNeighb-1)%numberOfNeighb);
         cg3::Vec3d vNext = t.getVertex((jPtr+1)%numberOfNeighb);

         cg3::Vec3d e1 = (vj-vNext);
         cg3::Vec3d e2 = (vi-vNext);
         cg3::Vec3d e3 = (vj-vPrev);
         cg3::Vec3d e4 = (vi-vPrev);

         e1.normalize();
         e2.normalize();
         e3.normalize();
         e4.normalize();

         double alpha = acos(e1.dot(e2));
         double beta = acos(e3.dot(e4));

         double cotAlpha = cot(alpha);
         double cotBeta = cot(beta);
         //testa cotAplha e cotBeta con std::isnan per verificare che siano validi

         //manca il termine 1/omega
         cg3::Vec3d delta = 0.5 * ((cotAlpha + cotBeta) * (vi-vj));
         differentialCoordinatesX[i] += delta.x();
         differentialCoordinatesY[i] += delta.y();
         differentialCoordinatesZ[i] += delta.z();
      }

   }*/

}

void MeshReconstructor::computeTrisAreas(const Trimesh & t,
                                         std::vector<double> & trisAreas)
{
   int numberOfTriangles = t.getNumTriangles();
   const std::vector<int> triangles = t.getTrianglesVector();
   for(int i = 0; i<numberOfTriangles; ++i)
   {
      int tPtr = i*3;
      cg3::Vec3d v1 = t.getVertex(triangles[tPtr+0]);
      cg3::Vec3d v2 = t.getVertex(triangles[tPtr+1]);
      cg3::Vec3d v3 = t.getVertex(triangles[tPtr+2]);

      cg3::Vec3d e1 = v2-v1;
      cg3::Vec3d e2 = v3-v1;
      trisAreas[i] = 0.5 * ( e1.cross(e2).length()); //IS IT CORRECT?!
   }
}





