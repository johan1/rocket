#ifndef _FREE_TYPE_LIB_H_
#define _FREE_TYPE_LIB_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>
#include <unordered_map>

#include "resource/ResourceId.h"

namespace rocket {

class FreeTypeLib {
public:
	static FT_Library get();

	static FT_Face lookupFace(rocket::resource::ResourceId const& font); 
	
private:
	class Face {
	public:
		Face(rocket::resource::ResourceId const& resourceId);
		~Face();
		FT_Face const& getFace() const;

	private:
		Face(Face const&) = delete;
		Face& operator=(Face const&) = delete;

		FT_Face face;
		std::vector<char> faceData;
	};

	FT_Library ftLib;
	std::unordered_map<rocket::resource::ResourceId, std::unique_ptr<Face>> faces;

	static FreeTypeLib& getInstance();

	FreeTypeLib();
	~FreeTypeLib();

	FT_Face lookupFaceImpl(rocket::resource::ResourceId const& font); 
};

}

#endif
