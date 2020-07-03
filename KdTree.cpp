//  KdTree Template - Simple KdTree class
//
//  SJSU  - CS134 Game Development
//
//  Kevin M. Smith   04/19/20

//  **Important:  Vertices (x, y, z) in the mesh are stored in the Tree node as an integer index.
//  to read the 3D vertex point from the mesh given index i,  use the function ofMesh::getVertex(i);  See
//  KdTree::meshBounds() for an example of usage;
//
//

#include "KdTree.h"
 

// draw KdTree (recursively)
//
void KdTree::draw(TreeNode & node, int numLevels, int level) {
  if (level >= numLevels) return;
  drawBox(node.box);
  level++;
  for (int i = 0; i < node.children.size(); i++) {
    draw(node.children[i], numLevels, level);
  }
}

// draw only leaf Nodes
//
void KdTree::drawLeafNodes(TreeNode & node, int numLevels, int level) {
    if (level >= numLevels) return;
    if(node.children.size() == 0)
    {
    drawBox(node.box);
     level++;
     for (int i = 0; i < node.children.size(); i++) {
    draw(node.children[i], numLevels, level);
     }
    }
    
}

//draw a box from a "Box" class
//
void KdTree::drawBox(const Box & box) {
  Vector3 min = box.parameters[0];
  Vector3 max = box.parameters[1];
  Vector3 size = max - min;
  Vector3 center = size / 2 + min;
  ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
  float w = size.x();
  float h = size.y();
  float d = size.z();
  ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box KdTree::meshBounds(const ofMesh & mesh) {
  int n = mesh.getNumVertices();
  ofVec3f v = mesh.getVertex(0);
  ofVec3f max = v;
  ofVec3f min = v;
  for (int i = 1; i < n; i++) {
    ofVec3f v = mesh.getVertex(i);

    if (v.x > max.x) max.x = v.x;
    else if (v.x < min.x) min.x = v.x;

    if (v.y > max.y) max.y = v.y;
    else if (v.y < min.y) min.y = v.y;

    if (v.z > max.z) max.z = v.z;
    else if (v.z < min.z) min.z = v.z;
  }
  //cout << "vertices: " << n << endl;
  //    cout << "min: " << min << "max: " << max << endl;
  return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained
//                      inside the Box.  Return count of points found;
//
int KdTree::getMeshPointsInBox(const ofMesh & mesh,
  const vector < int > & points,
    Box & box, vector < int > & pointsRtn) {
  int count = 0;
  for (int i = 0; i < points.size(); i++) {
    ofVec3f v = mesh.getVertex(points[i]);

    if (box.inside(Vector3(v.x, v.y, v.z))) {
      count++;

      pointsRtn.push_back(points[i]);

    }

  }
  return count;
}

//  Subdivide a Box; return children in  boxList

void KdTree::subDivideBox(const Box & box, vector < Box > & boxList,
  const vector < int > & points) {

  //get min and max for the box that is to be subdivided
  Vector3 min = box.parameters[0];
  Vector3 max = box.parameters[1];

  //Find the center value by subtracting min and max and dividing it by 2
  float centerX = (((min.x()) + (max.x())) / 2);
  float centerY = (((min.y()) + (max.y())) / 2);
  float centerZ = (((min.z()) + (max.z())) / 2);

  //creating all vector values to create boxes
  Vector3 c1 = Vector3(centerX, max.y(), max.z());
  Vector3 c2 = Vector3(centerX, min.y(), min.z());

  Vector3 y1 = Vector3(max.x(), centerY, max.z());
  Vector3 y2 = Vector3(min.x(), centerY, min.z());

  Vector3 z1 = Vector3(max.x(), max.y(), centerZ);
  Vector3 z2 = Vector3(min.x(), min.y(), centerZ);

  //set box values based upon different min and max values
  Box box1(min, c1);
  Box box2(c2, max);
  Box boxy(min, y1);
  Box boxy2(y2, max);
  Box boxz(min, z1);
  Box boxz2(z2, max);

  // use level variable to alternate between axis
  Box b[2];
  if (levelVariable % 3 == 0) {
    b[0] = boxy;
    b[1] = boxy2;
  } else if (levelVariable % 3 == 1) {
    b[0] = box1;
    b[1] = box2;
  } else {

    b[0] = boxz;
    b[1] = boxz2;
  }
  //add boxes to the boxList vector
  boxList.clear();
  for (int i = 0; i < 2; i++) {
    boxList.push_back(b[i]);
  }

}
//subdivide the box by using the median.
void KdTree::subDivideBoxMedian(const Box & box, vector < Box > & boxList,
  const vector < int > & points) {

  //get min and max for the box that is to be subdivided
  Vector3 min = box.parameters[0];
  Vector3 max = box.parameters[1];

    vector < float > axisX;
    
  // find the index of the median
  
    for(int i=0; i<mesh.getNumVertices();i++)
    {
         ofVec3f v2 = mesh.getVertex(i);
        axisX.push_back(v2.x);
    }
   
  
    
    int medianIndex = points.size() / 2;
    //get the vector position of the median
    ofVec3f v = mesh.getVertex(medianIndex);
    
  //create and find the center values
  float centerX = v.x;
  float centerY = v.y;
  float centerZ = v.z;

  //set box values based upon different min and max values
  Vector3 c1 = Vector3(centerX, max.y(), max.z());
  Vector3 c2 = Vector3(centerX, min.y(), min.z());

  Vector3 y1 = Vector3(max.x(), centerY, max.z());
  Vector3 y2 = Vector3(min.x(), centerY, min.z());

  Vector3 z1 = Vector3(max.x(), max.y(), centerZ);
  Vector3 z2 = Vector3(min.x(), min.y(), centerZ);

  //set box values based upon different min and max values
  Box box1(min, c1);
  Box box2(c2, max);
  Box boxy(min, y1);
  Box boxy2(y2, max);
  Box boxz(min, z1);
  Box boxz2(z2, max);

  // use level variable to alternate between axis
  Box b[2];
  if (levelVariable % 3 == 0) {
    b[0] = boxy;
    b[1] = boxy2;
  } else if (levelVariable % 3 == 1) {
      b[0] = box1;
      b[1] = box2;
  } else {

    b[0] = boxz;
    b[1] = boxz2;
  }

  //add boxes to the boxList vector
  boxList.clear();
  for (int i = 0; i < 2; i++) {
    boxList.push_back(b[i]);
  }

}
void KdTree::subDivideBox8(const Box &box, vector<Box> & boxList) {
    Vector3 min = box.parameters[0];
    Vector3 max = box.parameters[1];
    Vector3 size = max - min;
    Vector3 center = size / 2 + min;
    float xdist = (max.x() - min.x()) / 2;
    float ydist = (max.y() - min.y()) / 2;
    float zdist = (max.z() - min.z()) / 2;
    Vector3 h = Vector3(0, ydist, 0);

    //  generate ground floor
    //
    Box b[8];
    b[0] = Box(min, center);
    b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
    b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
    b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

    boxList.clear();
    for (int i = 0; i < 4; i++)
        boxList.push_back(b[i]);

    // generate second story
    //
    for (int i = 4; i < 8; i++) {
        b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
        boxList.push_back(b[i]);
    }
}


void KdTree::create(const ofMesh & geo, int numLevels) {
  // initialize KdTree structure
  //
  mesh = geo;
  int level = 0;

  root.box = meshBounds(mesh);
  for (int i = 0; i < mesh.getNumVertices(); i++) {
    root.points.push_back(i);
  }
  level++;

  subdivide(mesh, root, numLevels, level);

}

void KdTree::subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level) {
    if (level >= numLevels) return;
  //create all variables required to subdivide
  vector < Box > boxList;
  levelVariable = level;

  //subdivide this node using method from above
 // subDivideBox(node.box, boxList, node.points);
    subDivideBox8(node.box, boxList);
  level++;

  //add subdivided children to boxlist and get points and if there are more than 0 add to children vector

   for( int i = 0; i < boxList.size(); i++)
    {
        TreeNode child;
        child.box = boxList[i];
        int count = getMeshPointsInBox(mesh, node.points, child.box, child.points);
        if (count > 1) {
          node.children.push_back(child);
        }
    }
    
  for (int i = 0; i < node.children.size(); i++) {
    if (node.points.size() > 1) {
      subdivide(mesh, node.children[i], numLevels, level);

    }
  }
}

//intersect method select the smallest box in the trea
bool KdTree::intersect(const Ray & ray,
  const TreeNode & node, TreeNode & nodeRtn) {

  //initial check to see if the original click is inside the box if not return false
  if (node.box.intersect(ray, -1000, 1000)) {

    //check to see if the points in the box is less than 5 if so select that as your box
    // if not then run a loop on all of the children to find a box with less than 5 points

    if (node.children.size() == 0) {
      nodeRtn = node;
      return true;
    } else {
      for (int i = 0; i < node.children.size(); i++) {
        if (intersect(ray, node.children[i], nodeRtn)) {
          return true;
        }
        }
      }
    }
 else{
    return false;
  }
}

bool KdTree::intersect(const ofVec3f & vec, const TreeNode & node, TreeNode & nodeRtn) {
    Box bTest = node.box;
    if(bTest.inside(Vector3(vec.x, vec.y, vec.z))){
        if (node.children.size() == 0) {
            nodeRtn = node;
            return true;
        }
        else {
            for (int i = 0; i < node.children.size(); i++) {
                if (intersect(vec, node.children[i], nodeRtn)) {
                    return true;
                }
            }
        }
    }
    return false;
}
