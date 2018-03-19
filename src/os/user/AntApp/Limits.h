#ifndef __LIB_LIMITS_include__
#define __LIB_LIMITS_include__

/**
 * Limits for an area.
 * This struct stores the Limits of an area. There are methods to modify
 * a position in relation to the area.
 */
struct Limits {
	int left;
	int right;
	int top;
	int bottom;
	
	/**
	 * Area is like Border.
	 * The given x and y values are checked and it it is outside the limits,
	 * the values are modified like a Bounce.
	 * @param x check and modified x position
	 * @param y check and modified y position
	 */
	void likeBorder(int & x, int & y) {
		if (x < left) x=left;
		if (x >= right) x=right-1;
		if (y < top) y=top;
		if (y >= bottom) y=bottom-1;
	}
	
	/**
	 * Area is like World.
	 * The given x and y values are checked and it it is outside the limits,
	 * the values are modified like a endless map or world. if you leave the
	 * left side, you will appear at the right side.
	 * @param x check and modified x position
	 * @param y check and modified y position
	 */
	void likeWorld(int & x, int & y) {
		if (x < left) x=right-(left-x);
		if (x >= right) x=left+(x-right);
		if (y < top) y=bottom-(top-y);
		if (y >= bottom) y=top+(y-bottom);
	}
	
	/**
	 * Area is like a endless Paper.
	 * if you leave the area on the right, you jump to a new line. You can use
	 * it for VESA and printing characters.
	 * @param x check and modified x position
	 * @param y check and modified y position
	 * @param lineHeight the size of a new line
	 */
	void likeLines(int & x, int & y, const int & lineHeight) {
		if (x < left) {
			x=right-(left-x);
			y-=lineHeight;
		}
		if (x >= right) {
			x=left+(x-right);
			y+=lineHeight;
		}
		if (y < top) {
			y=bottom-(top-y);
		}
		if (y >= bottom) {
			y=top+(y-bottom);
		}
	}
};

#endif
