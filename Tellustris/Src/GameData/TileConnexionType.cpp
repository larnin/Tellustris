
#include "GameData/TileConnexionType.h"

#include <cassert>

TileConnexionType localMatrixToTileConnexionType(const FixedMatrix<bool, 3, 3> & mat)
{
	assert(mat(1, 1));

	bool l = mat(0, 1);
	bool r = mat(2, 1);
	bool t = mat(1, 0);
	bool d = mat(1, 2);
	bool tl = mat(0, 0);
	bool tr = mat(2, 0);
	bool dl = mat(0, 2);
	bool dr = mat(2, 2);

	//full
	if (!l && !r && !t && !d)
		return TileConnexionType::Full;
	//3 sides
	else if (!l && !r && !t)
		return TileConnexionType::Top3;
	else if (!l && !r && !d)
		return TileConnexionType::Down3;
	else if (!l && !t && !d)
		return TileConnexionType::Left3;
	else if (!r && !t && !d)
		return TileConnexionType::Right3;
	//horizontal vertical
	else if (!l && !r)
		return TileConnexionType::Vertical;
	else if (!t && !d)
		return TileConnexionType::Horizontal;
	//corner with corner
	else if (!l && !t && !dr)
		return TileConnexionType::DownRightWithCorner;
	else if (!l && !d && !tr)
		return TileConnexionType::TopRightWithCorner;
	else if (!r && !t && !dl)
		return TileConnexionType::DownLeftWithCorner;
	else if (!r && !d && !tl)
		return TileConnexionType::TopLeftWithCorner;
	//corner without corner
	else if (!l && !t)
		return TileConnexionType::TopLeft;
	else if (!l && !d)
		return TileConnexionType::DownLeft;
	else if (!r && !t)
		return TileConnexionType::TopRight;
	else if (!r && !d)
		return TileConnexionType::DownRight;
	//one border with 2 corners
	else if (!l && !dr && !tr)
		return TileConnexionType::LeftCorners2;
	else if (!r && !dl && !tl)
		return TileConnexionType::RightCorners2;
	else if (!t && !dl && !dr)
		return TileConnexionType::TopCorners2;
	else if (!d && !tl && !tr)
		return TileConnexionType::DownCorners2;
	//one border with one corner
	else if (!l && !dr)
		return TileConnexionType::LeftCornerDown;
	else if (!l && !tr)
		return TileConnexionType::LeftCornerTop;
	else if (!r && !dl)
		return TileConnexionType::RightCornerTop;
	else if (!r && !tl)
		return TileConnexionType::RightCornerDown;
	else if (!t && !dl)
		return TileConnexionType::TopCornerDown;
	else if (!t && !dr)
		return TileConnexionType::TopCornerTop;
	else if (!d && !tl)
		return TileConnexionType::DownCornerTop;
	else if (!d && !tr)
		return TileConnexionType::DownCornerDown;
	//one border without corners
	else if (!l)
		return TileConnexionType::Left;
	else if (!r)
		return TileConnexionType::Right;
	else if (!t)
		return TileConnexionType::Top;
	else if (!d)
		return TileConnexionType::Down;
	//4 corners
	else if (!tl && !tr && !dl && !dr)
		return TileConnexionType::QuadCorners;
	//3 corners
	else if (!tl && !tr && !dl)
		return TileConnexionType::TopLeftCorners3;
	else if (!tl && !tr && !dr)
		return TileConnexionType::TopRightCorners3;
	else if (!tl && !dl && !dr)
		return TileConnexionType::DownLeftCorners3;
	else if (!tr && !dl && !dr)
		return TileConnexionType::DownRightCorners3;
	//2 corners diagonal
	else if (!tr && !dl)
		return TileConnexionType::DiagonalTopRight;
	else if (!tl && !dr)
		return TileConnexionType::DiagonalTopLeft;
	//2 corners side
	else if (!tr && !dr)
		return TileConnexionType::RightCorners;
	else if (!tl && !dl)
		return TileConnexionType::LeftCorners;
	else if (!tr && !tl)
		return TileConnexionType::TopCorners;
	else if (!dr && !dl)
		return TileConnexionType::DownCorners;
	//1 corner
	else if (!tr)
		return TileConnexionType::TopRightCorner;
	else if (!tl)
		return TileConnexionType::TopLeftCorner;
	else if (!dr)
		return TileConnexionType::DownRightCorner;
	else if (!dl)
		return TileConnexionType::DownLeftCorner;
	//empty
	else return TileConnexionType::Empty;
}
