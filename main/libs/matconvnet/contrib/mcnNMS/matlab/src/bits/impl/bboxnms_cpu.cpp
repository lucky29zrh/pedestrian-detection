// @file bboxnms_cpu.cu
// @brief Bounding Box non maximum supression, based 
// on Shaoqing Ren's Faster R-CNN implementation which 
// can be found here: 
// https://github.com/ShaoqingRen/faster_rcnn/blob/master/functions/nms
// @author Samuel Albanie

/*
Copyright (C) 2017- Samuel Albanie.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include "bboxnms.hpp"
#include <bits/data.hpp>
#include <bits/mexutils.h>

#include <assert.h>
#include <float.h>
#include <cstdio>
#include <math.h>
#include <string.h>

#include <algorithm>
#include <vector>
#include <map>

namespace vl { namespace impl {

  template<typename T>
  struct bboxnms<vl::VLDT_CPU,T>
  {

    static vl::ErrorCode
    forward(Context& context,
            std::vector<int> &output,
            T const* boxes,
            float overlap, 
            size_t num_boxes,
            int &num_kept)
    {
      std::multimap<T, int> scores ;
      std::vector<double> boxAreas(num_boxes) ;

      for (int ii = 0 ; ii < num_boxes ; ++ii) {
          boxAreas[ii] = double(boxes[ii*5 + 2] - boxes[ii*5] + 1 ) * 
                 (boxes[ii*5 + 3] - boxes[ii*5 + 1] + 1 ) ;
          scores.insert(std::pair<T,int>(boxes[ii*5 + 4], ii)) ;
          if (boxAreas[ii] < 0) 
              vlmxError(VLMXE_IllegalArgument, "All box areas should be > 0") ;
      }

      while (scores.size() > 0) {
          int last = scores.rbegin()->second ;
          output[num_kept] = last ;
          num_kept += 1 ;
          T last_xmin = boxes[last*5] ;
          T last_ymin = boxes[last*5 + 1] ;
          T last_xmax = boxes[last*5 + 2] ;
          T last_ymax = boxes[last*5 + 3] ;

          for (typename std::multimap<T,int>::iterator it = scores.begin() ; 
                                                       it != scores.end() ;) {
              int idx = it->second ;
              T x1 = std::max(last_xmin, boxes[idx*5]) ;
              T y1 = std::max(last_ymin, boxes[idx*5 + 1]) ;
              T x2 = std::min(last_xmax, boxes[idx*5 + 2]) ;
              T y2 = std::min(last_ymax, boxes[idx*5 + 3]) ;
              double w = std::max(T(0), x2-x1+1) ; 
              double h = std::max(T(0), y2-y1+1) ;
              double ov = w*h / (boxAreas[last] + boxAreas[idx] - w*h) ;
              if (ov > overlap) {
                  it = scores.erase(it) ;
              } else {
                  it++ ;
              }
          }
      }
      return VLE_Success ;
   }
 } ;
} } // namespace vl::impl

template struct vl::impl::bboxnms<vl::VLDT_CPU, float> ;

#ifdef ENABLE_DOUBLE
template struct vl::impl::bboxnms<vl::VLDT_CPU, double> ;
#endif
