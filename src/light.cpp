
#include "light.h"
#include "shape.h"
#include "camera.h"
#include <cmath>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

/*
 * XXXaokblast: The tree may need to be rebuilt if textureX, textureY, or yaw,
 * pitch, raw changes between frames. This does not occur in the instructor's
 * test cases, so we can build the tree once after inserting all shapes.
 */
class BVHTree {
  constexpr static int NBUCKETS = 12;
  constexpr static int LEAF_SIZE = 4;
  struct BVHTreeNode {
    AABB aabb;
    int left, right;
    bool is_leaf;
    std::vector<Shape *> leafs;
    BVHTreeNode(AABB aabb, int left, int right)
        : aabb(aabb), left(left), right(right), is_leaf(false) {};
    BVHTreeNode(AABB aabb, std::vector<Shape *> shapes)
        : aabb(aabb), leafs(std::move(shapes)), left(-1), right(-1),
          is_leaf(true) {};
    BVHTreeNode() : left(-1), right(-1), is_leaf(false) {}
  };
  std::vector<BVHTreeNode> root;
  std::vector<Shape *> unbounded;

  static inline void
  buildLeaf(std::vector<std::unique_ptr<Shape>> &shapes, const std::vector<AABB> &aabbs,
            const std::vector<int> &iotas, BVHTreeNode *cur) {
	AABB aabb;
	std::vector<Shape *> leafShapes;
	leafShapes.reserve(iotas.size());
	for (const int idx : iotas) {
		aabb.Union(aabbs[idx]);
		leafShapes.push_back(shapes[idx].get());
	}
	*cur = BVHTreeNode(aabb, std::move(leafShapes));
  }

  static int
  buildImpl(std::vector<std::unique_ptr<Shape>> &shapes, const std::vector<AABB> &aabbs, const std::vector<int> &iotas, std::vector<BVHTreeNode> &root) {
	if (iotas.size() <= LEAF_SIZE) {
		int cur_idx = root.size();
		root.push_back({});
		buildLeaf(shapes, aabbs, iotas, &root[cur_idx]);
		return cur_idx;
	}            
    AABB aabb = aabbs[iotas[0]];
    for (size_t i = 1; i < iotas.size(); ++i)
      aabb.Union(aabbs[iotas[i]]);
    int max_axe = 0;
    for (int i = 1; i < 3; ++i)
      if ((aabb.maxbounds[i] - aabb.minbounds[i]) >
          (aabb.maxbounds[max_axe] - aabb.minbounds[max_axe]))
        max_axe = i;

    const double lo = aabb.minbounds[max_axe];
    const double intervals = aabb.maxbounds[max_axe] - lo;
    std::vector<int> liota, riota;

    // An AABB may collapse to a single point when it contains only one point.
    // Make sure it has non-zero extent.
    if (intervals > 0) {
      std::vector<int> buckets[NBUCKETS];
      for (size_t i = 0; i < iotas.size(); ++i) {
        int bucketIdx = NBUCKETS * (aabbs[iotas[i]].minbounds[max_axe] - lo) / intervals;
        if (bucketIdx >= NBUCKETS)
          bucketIdx = NBUCKETS - 1;
        buckets[bucketIdx].push_back(iotas[i]);
      }

      double minCost = std::numeric_limits<double>::max();
      int minIdx = -1;
      int lcnt = 0;
      AABB lsurface;

      for (int i = 0; i < NBUCKETS - 1; ++i) {
        lcnt += (int)buckets[i].size();
        for (const int idx : buckets[i])
          lsurface.Union(aabbs[idx]);
        int rcnt = 0;
        AABB rsurface;
        for (int j = i + 1; j < NBUCKETS; ++j) {
          for (const int idx : buckets[j])
            rsurface.Union(aabbs[idx]);
          rcnt += (int)buckets[j].size();
        }
        if (lcnt == 0 || rcnt == 0)
          continue;
        // nA * pA + nB * pB
        double cost = lsurface.calcSurfaceArea() * lcnt +
                      rsurface.calcSurfaceArea() * rcnt;
        if (cost < minCost) {
          minCost = cost;
          minIdx = i;
        }
      }

      if (minIdx >= 0) {
        for (int i = 0; i <= minIdx; ++i)
          for (const int idx : buckets[i])
            liota.push_back(idx);
        for (int i = minIdx + 1; i < NBUCKETS; ++i)
          for (const int idx : buckets[i])
            riota.push_back(idx);
      }
    }

    // Fall back to an even split if we cannot find a valid split or all shapes
    // fall into the same bucket.
    if (liota.empty() || riota.empty()) {
      const size_t mid = iotas.size() / 2;
      liota.assign(iotas.begin(), iotas.begin() + mid);
      riota.assign(iotas.begin() + mid, iotas.end());
    }
	int cidx = root.size();
	root.push_back({});
    int lidx = buildImpl(shapes, aabbs, liota, root);
    int ridx = buildImpl(shapes, aabbs, riota, root);
    aabb = root[lidx].aabb;
    aabb.Union(root[ridx].aabb);
	root[cidx] = BVHTreeNode(aabb, lidx, ridx);
    return cidx;
  }

  static void intersectLeaf(const BVHTreeNode *root, const Ray &ray,
                            Shape *&shape, double &tmin) {
    for (auto cshape : root->leafs) {
      double ctime = cshape->getIntersection(ray);
      if (ctime < tmin) {
        tmin = ctime;
        shape = cshape;
      }
    }
  }

  static bool aabbIntersect(const AABB &aabb, const Ray &ray, double &tmin) {
    Vector inv_dir = 1.0 / ray.vector;
    double mn = std::numeric_limits<double>::lowest();
    double mx = std::numeric_limits<double>::max();

    double ta = (aabb.minbounds[0] - ray.point.x) * inv_dir.x;
    double tb = (aabb.maxbounds[0] - ray.point.x) * inv_dir.x;
    mn = std::max(mn, std::min(ta, tb));
    mx = std::min(mx, std::max(ta, tb));

    ta = (aabb.minbounds[1] - ray.point.y) * inv_dir.y;
    tb = (aabb.maxbounds[1] - ray.point.y) * inv_dir.y;
    mn = std::max(mn, std::min(ta, tb));
    mx = std::min(mx, std::max(ta, tb));

    ta = (aabb.minbounds[2] - ray.point.z) * inv_dir.z;
    tb = (aabb.maxbounds[2] - ray.point.z) * inv_dir.z;
    mn = std::max(mn, std::min(ta, tb));
    mx = std::min(mx, std::max(ta, tb));

    if (mx >= std::max(mn, 0.0)) {
      tmin = mn;
      return true;
    }
    return false;
  }

  static void intersectImpl(std::vector<BVHTreeNode> &root, const Ray &ray,
                            double &tres, Shape *&shape) {
    // {node, tmin}
    std::vector<std::pair<int, double>> stack;
    double tclose = inf;
    double tmin;

    shape = NULL;
    if (aabbIntersect(root[0].aabb, ray, tmin))
      stack.push_back({0, tmin});

    while (stack.size()) {
      auto [cidx, ctmin] = stack.back();
      stack.pop_back();
	  BVHTreeNode *cur = &root[cidx];

      if (ctmin > tclose)
        continue;
      if (cur->is_leaf) {
        intersectLeaf(cur, ray, shape, tclose);
        continue;
      }
      double ltmin, rtmin;
      bool lhit = aabbIntersect(root[cur->left].aabb, ray, ltmin) && ltmin <= tclose;
      bool rhit =
          aabbIntersect(root[cur->right].aabb, ray, rtmin) && rtmin <= tclose;
      if (lhit && rhit) {
        if (ltmin < rtmin) {
          stack.push_back({cur->right, rtmin});
          stack.push_back({cur->left, ltmin});
        } else {
          stack.push_back({cur->left, ltmin});
          stack.push_back({cur->right, rtmin});
        }
      } else if (lhit)
        stack.push_back({cur->left, ltmin});
      else if (rhit)
        stack.push_back({cur->right, rtmin});
    }
    tres = tclose;
  }

public:
  const std::vector<Shape *> &getUnbounded() const { return unbounded; }

  void buildFromShapes(std::vector<std::unique_ptr<Shape>> &shapes) {
    std::vector<AABB> aabbs(shapes.size());
    std::vector<int> iotas;
    iotas.reserve(shapes.size());
    unbounded.clear();

    for (size_t i = 0; i < shapes.size(); ++i) {
      aabbs[i] = shapes[i]->buildAABB();
      // Unbounded planes are excluded from the BVH build because they have
      // infinite bounding intervals. We must intersect these objects linearly.
      if (aabbs[i].isFinite())
        iotas.push_back((int)i);
      else
        unbounded.push_back(shapes[i].get());
    }

	root.clear();
	if (iotas.size()) {
		root.reserve(2 * iotas.size() - 1);
		buildImpl(shapes, aabbs, iotas, root);
		root.shrink_to_fit();
	}          

  }

  double intersect(const Ray lightRay, Shape *&shape) {
    double tclosest = inf;
    shape = NULL;
    if (root.size())
      intersectImpl(root, lightRay, tclosest, shape);
    // Unbounded shapes are not in the tree, so they must be tested linearly.
    for (Shape *cshape : unbounded) {
      double ctime = cshape->getIntersection(lightRay);
      if (ctime < tclosest) {
        tclosest = ctime;
        shape = cshape;
      }
    }
    return tclosest;
  }
};
      
Light::Light(const Vector & cente, unsigned char* colo) : center(cente){
   color = colo;
}

unsigned char* Light::getColor(unsigned char a, unsigned char b, unsigned char c){
   unsigned char* r = (unsigned char*)malloc(sizeof(unsigned char)*3);
   r[0] = a;
   r[1] = b;
   r[2] = c;
   return r;
}

Autonoma::Autonoma(const Camera& c): camera(c){
   depth = 10;
   skybox = BLACK;
}

Autonoma::Autonoma(const Camera& c, Texture* tex): camera(c){
   depth = 10;
   skybox = tex;
}

void Autonoma::addShape(Shape* r){
	shapes.push_back(std::unique_ptr<Shape>(r));
}

void Autonoma::addLight(Light* r){
	lights.push_back(std::unique_ptr<Light>(r));
}

void Autonoma::buildBVHTree() {
  if (!bvh)
    bvh = new BVHTree();
  bvh->buildFromShapes(shapes);
}

void Autonoma::intersect(const Ray ray, Shape *&shape, double &time) {
  time = bvh->intersect(ray, shape);
}

Autonoma::~Autonoma() { delete bvh; };

void getLight(double* tColor, Autonoma* aut, Vector point, Vector norm, unsigned char flip){
   tColor[0] = tColor[1] = tColor[2] = 0.;
   for (const auto &t : aut->lights) {
      double lightColor[3];     
      lightColor[0] = t->color[0]/255.;
      lightColor[1] = t->color[1]/255.;
      lightColor[2] = t->color[2]/255.;
      Vector ra = t->center-point;
      bool hit = false;
      Ray lightRay = Ray(point+ra*.01, ra);
	  for (const auto &shape : aut->shapes) {
		 hit = shape->getLightIntersection(lightRay, lightColor);
		 if (hit)
			 break;                   
      }
      if(!hit){
         double perc_dot = norm.dot(ra);
         if(flip && perc_dot<0) perc_dot=-perc_dot;
         if(perc_dot>0){
            double perc = perc_dot/(ra.mag());
            tColor[0]+= perc*(lightColor[0]);
            tColor[1]+= perc*(lightColor[0]);
            tColor[2]+= perc*(lightColor[0]);
            if(tColor[0]>1.) tColor[0] = 1.;
            if(tColor[1]>1.) tColor[1] = 1.;
            if(tColor[2]>1.) tColor[2] = 1.;
         }
      }
   }
}
