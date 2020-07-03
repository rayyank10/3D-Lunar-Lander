//  KdTree Template - Simple KdTree class
//
//  SJSU  - CS134 Game Development
//
//  Kevin M. Smith   04/19/20

#pragma once
#include "ofMain.h"
#include "box.h"
#include "ray.h"
#include <iostream>
#include <algorithm>
#include <vector>


//  General purpose TreeNode class
//

class TreeNode {
public:
	Box box;
	vector<int> points;
	vector<TreeNode> children;    // for binary KdTree, this array has just two(2) members)
};

class KdTree {
public:
	
	// Create and initilize tree given an OpenFrameWorks Mesh and # of levels.  if numLevels
	// is larger than the number of levels possible in the data, then the tree will be built to
	// its maximum depth with only one point in each leaf node.
	//
	void create(const ofMesh & mesh, int numLevels);

	//  Recursive call to subdivide mesh starting at the "node"
	//
	void subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int currentLevel);

	//  Test for intersection of Tree given a ray and root node.  If a node is hit, return true
	//  and the correspondind node in "nodeRtn"
	//
	bool intersect(const Ray &, const TreeNode & node, TreeNode & nodeRtn);
    bool intersect(const ofVec3f & vec, const TreeNode & node, TreeNode & nodeRtn);

	//  Recursive calls to draw the tree
	//
	void draw(TreeNode & node, int numLevels, int level);
	void draw(int numLevels, int level) {
		draw(root, numLevels, level);
	}

	// Draw just the leaf nodes
	//  
	void drawLeafNodes(TreeNode & node, int numLevels, int level);

	// utility function to draw a box in OF from a "Box" class
	//
	static void drawBox(const Box &box);

	//  Returns a bounding Box for the mesh
	//
	static Box meshBounds(const ofMesh &);

	//  Test which mesh points in "points" are inside "box"; return inside box in "pointsRtn"
	//
	int getMeshPointsInBox(const ofMesh &mesh, const vector<int> & points, Box & box, vector<int> & pointsRtn);

	//   subdvide a Box into separate boxes.  for a binary tree, subdivide into two (2) boxes.
	//
	void subDivideBox(const Box &b, vector<Box> & boxList, const vector<int> & points);
    
    void subDivideBoxMedian(const Box &b, vector<Box> & boxList, const vector<int> & points);
    void subDivideBox8(const Box &box, vector<Box> & boxList);

	//  local data
    //
	ofMesh mesh;
	TreeNode root;
    int levelVariable; 
};
