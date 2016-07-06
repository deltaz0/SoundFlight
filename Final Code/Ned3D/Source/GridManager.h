
#ifndef __GRIDMANAGER_H_INCLUDED__
#define __GRIDMANAGER_H_INCLUDED__

#include "Objects\GameObject.h"
#include <d3dx9.h>

class GridManager {

public:
  GridManager(Vector3 p_vPlaneSpawn, EulerAngles p_PlaneOrient);
  ~GridManager(void);

  // Public Render Function
  void renderEntireGrid(float p_fPosZ); // Single function that calls all other render functions

  // Return Functions
	bool getGridActive(int lane);

	int getLaneColorRed(int lane);
	int getLaneColorGreen(int lane);
	int getLaneColorBlue(int lane);
	int getLaneHue(int lane);

	int numBeatsToActivateLane;
	float zPosSuccessStart[4];
	float zPosActiveStart[4];
	float zPosActiveEnd[4];

	int prevNode[4];

	void initVars();

	int score;
	int scoreMultiplier;
	int addScore();
	int updateMultiplier();

private:

	struct gridActive {
		bool isActive;
		Vector3 gridActiveVerts[4];
	};

	gridActive m_gridActive[4];

	struct argbcolor {
		int alpha;
		int red;
		int green;
		int blue;
	};

	argbcolor laneColor[4];

	//0 to 360
	int laneHue[4];

  // Init Functions
  void initGrid();  // Calls all init functions
  void initGridActive();
  void initGridBeats();
  void initGridEdges();

  // Render Functions
  void renderActiveGrids();
  void renderGridEdges();
  void renderGridBeats(float f_pPosZ);

	int renderProgressGrid();

  // Maintenance functions
  void checkActiveGrids(float p_fPosZ);
  int deActivateGrid(int lane);
  int activateGrid(int lane, int numBeats);

	void updateColors();
	

  // Textures
  int m_nGridActiveTexture;
  int m_nGridBeatTexture;
  int m_nGridSideTexture;
	int m_nGridShadowTexture;
	int m_nGridProgressTexture;
	int m_nGridProgGoalTexture;

  // Render Vertexes for the edges
  RenderVertexL m_vBottomRender[4];
  RenderVertexL m_vLeftRender[4];
  RenderVertexL m_vRightRender[4];
  RenderVertexL m_vTopRender[4];

	D3DXVECTOR3 tve;
	D3DXVECTOR4 pve;
	D3DXMATRIX mve;

  // Render Vertex Array for Beats
  RenderVertexL m_vBeats[4][4][4];

  // Misc. Members
  EulerAngles m_PlaneOrient;
  Vector3 m_vPlaneSpawn;

};


#endif