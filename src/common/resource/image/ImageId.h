#ifndef _ROCKET2D_IMAGE_ID_H_
#define _ROCKET2D_IMAGE_ID_H_

#include "../ResourceId.h"

namespace rocket { namespace resource { namespace image {

class ImageId {
public:
	explicit ImageId(ResourceId const& resId) : resId(resId), x(0.0f), y(0.0f), width(1.0f), height(1.0f) {}

	/*! (x,y) offset in normalized coordinates, where (0,0) is the lower left of the image.
 	 *  (width, height) is the width and height in normalized coordinates.
 	 */ 
	ImageId(ResourceId const& resId, float x, float y, float width, float height) :
			resId(resId), x(x), y(y), width(width), height(height) {}

	ResourceId const& getResourceId() const { return resId; } //! resource id
	float getX() const { return x; } //! x offset in normalized coordinates
	float getY() const { return y; } //! y offset in normalized coordinates
	float getWidth() const { return width; } //! width in normalized coordinates 
	float getHeight() const { return height; } //! height in normalized coordinates.

private:
	ResourceId resId;
	float x;
	float y;
	float width;
	float height;
};

}}

using namespace resource::image; // Collapse
}

#endif // _ROCKET2D_IMAGE_ID_H_
