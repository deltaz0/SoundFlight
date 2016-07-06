#include "GridManager.h"

#include "Game.h"
#include "Input\Input.h"
#include "Common\RotationMatrix.h"
#include <d3dx9.h>

static const float m_fGridWidth = 30.0f;
const float m_fGridFrontZOffset = 100000.0f;
const float m_fGridBackZOffset = 60.0f;

GridManager::GridManager(Vector3 p_vPlaneSpawn, EulerAngles p_PlaneOrient) {

  /*
  GridObject t_oGrid(NULL);
  t_oGrid.setPosition(p_vPlaneSpawn);
  t_oGrid.createGridFromPosition();
  */

  m_vPlaneSpawn = p_vPlaneSpawn;
  m_PlaneOrient = p_PlaneOrient;

	// Load Textures
  m_nGridActiveTexture = gRenderer.cacheTextureDX("gridactivetexture.png");
  m_nGridBeatTexture = gRenderer.cacheTextureDX("gridbeattexture.png");
  m_nGridSideTexture = gRenderer.cacheTextureDX("testtexture.png");
	m_nGridShadowTexture = gRenderer.cacheTextureDX("gridshadowtexture.png");
	m_nGridProgressTexture = gRenderer.cacheTextureDX("gridprogresstexture.png");
	m_nGridProgGoalTexture = gRenderer.cacheTextureDX("gridproggoaltexture.png");

  // Initialize Grid
  initGrid();

  // TEMP - Test activation
  //activateGrid(0,24);
	//activateGrid(1,50);
	//activateGrid(2,50);
	//activateGrid(3,50);

	

}


GridManager::~GridManager(void) {
	/*
	delete mve;
	delete pve;
	delete tve;
	*/
}

void GridManager::initVars(){
	initGrid();
}

void GridManager::initGrid() {

  // Setup Grid Lanes
  initGridActive();
  // End - Setup Grid Lanes

	//initialize colors
	for(int i=0; i<4; i++){
		laneColor[i].alpha = 255;
		laneColor[i].red = laneColor[i].green
		= laneColor[i].blue = 255;
		laneHue[i] = 0;
	}

	for(int zp=0;zp<4;zp++){
	zPosActiveStart[zp] = 0.0f;
	zPosActiveEnd[zp] = 0.0f;
	}

	prevNode[0] = 0;
	prevNode[1] = 0;
	prevNode[2] = 0;
	prevNode[3] = 0;

	numBeatsToActivateLane = 8;
	zPosSuccessStart[0] = 0.0f;
	zPosSuccessStart[1] = 0.0f;
	zPosSuccessStart[2] = 0.0f;
	zPosSuccessStart[3] = 0.0f;

	tve = D3DXVECTOR3(0,0,0);
	pve = D3DXVECTOR4(0,0,0,0);
	mve = D3DXMATRIX(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

	score = 0;
	scoreMultiplier = 0;

  // Setup quad
  float size = 1.3f;
  RotationMatrix t_RotMatrix;
  t_RotMatrix.setup(m_PlaneOrient);
  Vector3 right = Vector3::kRightVector * size;
  Vector3 up = Vector3::kUpVector * size;

  right = t_RotMatrix.objectToInertial(right);
  up = t_RotMatrix.objectToInertial(up);

  RenderVertexL t_vSetup[4];
  t_vSetup[0].p = m_vPlaneSpawn - right + up; 
  t_vSetup[0].u = 0.0f; t_vSetup[0].v = 0.0f;
  t_vSetup[1].p = m_vPlaneSpawn + right + up;
  t_vSetup[1].u = 1.0f; t_vSetup[1].v = 0.0f;
  t_vSetup[2].p = m_vPlaneSpawn + right - up;
  t_vSetup[2].u = 1.0f; t_vSetup[2].v = 1.0f;
  t_vSetup[3].p = m_vPlaneSpawn - right - up;
  t_vSetup[3].u = 0.0f; t_vSetup[3].v = 1.0f;

  // Setup Grid Edges
  for(int i=0; i<4; i++)
    m_vBottomRender[i] = m_vLeftRender[i] = m_vRightRender[i] = m_vTopRender[i] = t_vSetup[i];

  initGridEdges();
  // End - Setup Grid Edges

  // Setup Grid Beats
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			for(int k = 0; k < 4; k++) {
        m_vBeats[i][j][k].p = t_vSetup[k].p;
				m_vBeats[i][j][k].u = t_vSetup[k].u;
				m_vBeats[i][j][k].v = t_vSetup[k].v;
			}
    }
  }

  initGridBeats();
  // End - Setup Grid Beats

}

void GridManager::initGridActive() {

  // Initialize all gridActives to false
  for(int count = 0; count < 4; count++)
    m_gridActive[count].isActive = false;

  float tLX = m_vPlaneSpawn.x - ((m_fGridWidth + 6.1f) * 1.0f);
	float tRX = m_vPlaneSpawn.x + ((m_fGridWidth + 6.1f) * 1.0f);
	float tDY = m_vPlaneSpawn.y - ((m_fGridWidth + 6.1f) * 1.0f);
	float tUY = m_vPlaneSpawn.y + ((m_fGridWidth + 6.1f) * 1.0f);

	m_gridActive[0].gridActiveVerts[0] = Vector3(tLX,tDY,0);
	m_gridActive[0].gridActiveVerts[1] = Vector3(tRX,tDY,0);
	m_gridActive[0].gridActiveVerts[2] = Vector3(tRX,tDY,0);
	m_gridActive[0].gridActiveVerts[3] = Vector3(tLX,tDY,0);
	
	m_gridActive[1].gridActiveVerts[0] = Vector3(tRX,tDY,0);
	m_gridActive[1].gridActiveVerts[1] = Vector3(tRX,tUY,0);
	m_gridActive[1].gridActiveVerts[2] = Vector3(tRX,tUY,0);
	m_gridActive[1].gridActiveVerts[3] = Vector3(tRX,tDY,0);

	m_gridActive[2].gridActiveVerts[0] = Vector3(tRX,tUY,0);
	m_gridActive[2].gridActiveVerts[1] = Vector3(tLX,tUY,0);
	m_gridActive[2].gridActiveVerts[2] = Vector3(tLX,tUY,0);
	m_gridActive[2].gridActiveVerts[3] = Vector3(tRX,tUY,0);

	m_gridActive[3].gridActiveVerts[0] = Vector3(tLX,tUY,0);
	m_gridActive[3].gridActiveVerts[1] = Vector3(tLX,tDY,0);
	m_gridActive[3].gridActiveVerts[2] = Vector3(tLX,tDY,0);
	m_gridActive[3].gridActiveVerts[3] = Vector3(tLX,tUY,0);

}

void GridManager::initGridBeats() {

  // NOTE - This value must be SMALLER than the value in initGridEdges
  float t_fExtendGrid = m_fGridWidth + 6.0f;

  Vector3 t_vBottomY, t_vTopY, t_vLeftX, t_vRightX, t_vOffsetX, t_vOffsetY, t_vOffsetZBack, t_vOffsetZFront;

  t_vBottomY = Vector3(0, m_vPlaneSpawn.y - t_fExtendGrid, m_vPlaneSpawn.z);
  t_vTopY = Vector3(0, m_vPlaneSpawn.y + t_fExtendGrid, m_vPlaneSpawn.z);
  t_vLeftX = Vector3(0 - t_fExtendGrid, m_vPlaneSpawn.y, m_vPlaneSpawn.z);
  t_vRightX = Vector3(0 + t_fExtendGrid, m_vPlaneSpawn.y, m_vPlaneSpawn.z);

  t_vOffsetX = Vector3(t_fExtendGrid, 0, 0);
  t_vOffsetY = Vector3(0, t_fExtendGrid, 0);
  t_vOffsetZBack = Vector3(0, 0, m_fGridBackZOffset);
  t_vOffsetZFront = Vector3(0, 0, m_fGridFrontZOffset);

	for(int i=0; i<4; i++) {
		
    m_vBeats[i][0][0].p = t_vBottomY - t_vOffsetX;
		m_vBeats[i][0][1].p = t_vBottomY + t_vOffsetX;
		m_vBeats[i][0][2].p = t_vBottomY + t_vOffsetX;
		m_vBeats[i][0][3].p = t_vBottomY - t_vOffsetX;

		m_vBeats[i][1][0].p = t_vTopY + t_vOffsetX;
		m_vBeats[i][1][1].p = t_vTopY - t_vOffsetX;
		m_vBeats[i][1][2].p = t_vTopY - t_vOffsetX;
    m_vBeats[i][1][3].p = t_vTopY + t_vOffsetX;

		m_vBeats[i][2][0].p = t_vLeftX + t_vOffsetY;
		m_vBeats[i][2][1].p = t_vLeftX - t_vOffsetY;
		m_vBeats[i][2][2].p = t_vLeftX - t_vOffsetY;
		m_vBeats[i][2][3].p = t_vLeftX + t_vOffsetY;

		m_vBeats[i][3][0].p = t_vRightX - t_vOffsetY;
		m_vBeats[i][3][1].p = t_vRightX + t_vOffsetY;
		m_vBeats[i][3][2].p = t_vRightX + t_vOffsetY;
		m_vBeats[i][3][3].p = t_vRightX - t_vOffsetY;

	}

}

void GridManager::initGridEdges() {

  // NOTE - This value must be LARGER than the one in initGridBeats
  float t_fExtendGrid = m_fGridWidth + 8.0f;

  Vector3 t_vBottomY, t_vTopY, t_vLeftX, t_vRightX, t_vOffsetX, t_vOffsetY, t_vOffsetZBack, t_vOffsetZFront;

  t_vBottomY = Vector3(0, m_vPlaneSpawn.y - t_fExtendGrid, m_vPlaneSpawn.z);
  t_vTopY = Vector3(0, m_vPlaneSpawn.y + t_fExtendGrid, m_vPlaneSpawn.z);
  t_vLeftX = Vector3(0 - t_fExtendGrid, m_vPlaneSpawn.y, m_vPlaneSpawn.z);
  t_vRightX = Vector3(0 + t_fExtendGrid, m_vPlaneSpawn.y, m_vPlaneSpawn.z);

  t_vOffsetX = Vector3(t_fExtendGrid, 0, 0);
  t_vOffsetY = Vector3(0, t_fExtendGrid, 0);
  t_vOffsetZBack = Vector3(0, 0, m_fGridBackZOffset);
  t_vOffsetZFront = Vector3(0, 0, m_fGridFrontZOffset);

  m_vBottomRender[0].p = t_vBottomY - t_vOffsetX + t_vOffsetZFront;
	m_vBottomRender[1].p = t_vBottomY + t_vOffsetX + t_vOffsetZFront;
  m_vBottomRender[2].p = t_vBottomY + t_vOffsetX - t_vOffsetZBack;
	m_vBottomRender[3].p = t_vBottomY - t_vOffsetX - t_vOffsetZBack;

	m_vTopRender[0].p = t_vTopY + t_vOffsetX + t_vOffsetZFront;
	m_vTopRender[1].p = t_vTopY - t_vOffsetX + t_vOffsetZFront;
	m_vTopRender[2].p = t_vTopY - t_vOffsetX - t_vOffsetZBack;
	m_vTopRender[3].p = t_vTopY + t_vOffsetX - t_vOffsetZBack;

	m_vLeftRender[0].p = t_vLeftX + t_vOffsetY + t_vOffsetZFront;
	m_vLeftRender[1].p = t_vLeftX - t_vOffsetY + t_vOffsetZFront;
	m_vLeftRender[2].p = t_vLeftX - t_vOffsetY - t_vOffsetZBack;
  m_vLeftRender[3].p = t_vLeftX + t_vOffsetY - t_vOffsetZBack;

	m_vRightRender[0].p = t_vRightX - t_vOffsetY + t_vOffsetZFront;
	m_vRightRender[1].p = t_vRightX + t_vOffsetY + t_vOffsetZFront;
	m_vRightRender[2].p = t_vRightX + t_vOffsetY - t_vOffsetZBack;
  m_vRightRender[3].p = t_vRightX - t_vOffsetY - t_vOffsetZBack;

  // set up color
  for(int a = 0; a < 4; a++) m_vBottomRender[a].argb = MAKE_ARGB(255,100,100,100);
  for(int a = 0; a < 4; a++) m_vTopRender[a].argb = MAKE_ARGB(255,100,100,100);
  for(int a = 0; a < 4; a++) m_vLeftRender[a].argb = MAKE_ARGB(255,100,100,100);
  for(int a = 0; a < 4; a++) m_vRightRender[a].argb = MAKE_ARGB(255,100,100,100);

}

void GridManager::renderEntireGrid(float p_fPosZ) {

  /*
  if(gInput.keyJustDown(DIK_SPACE)) {
    activateGrid(0, 8);
    }
    */
	updateColors();
  checkActiveGrids(p_fPosZ);
  renderActiveGrids();
  renderGridEdges();
  renderGridBeats(p_fPosZ);
	renderProgressGrid();
	updateMultiplier();

	for(int xag=0;xag<4;xag++){
		if(getGridActive(xag)){
			gGame.m_statePlaying.adjustXSoundVolumeByActiveLane(xag,true);
		}
	}
	
	RenderVertexL nodeVerts[6][4];
	RenderVertexL nodeShadowVerts[4];
	//RenderVertexL planeBoxCheck[4];

	for(int j=0; j<6; j++){
    nodeVerts[j][0].u = 0.0f; nodeVerts[j][0].v = 0.0f;
    nodeVerts[j][1].u = 1.0f; nodeVerts[j][1].v = 0.0f;
    nodeVerts[j][2].u = 1.0f; nodeVerts[j][2].v = 1.0f;
    nodeVerts[j][3].u = 0.0f; nodeVerts[j][3].v = 1.0f;
	}
	for(int f=0; f<4; f++){
		nodeShadowVerts[f].u = nodeVerts[0][f].u;
		nodeShadowVerts[f].v = nodeVerts[0][f].v;
		//planeBoxCheck[f].u = nodeVerts[0][f].u;
		//planeBoxCheck[f].v = nodeVerts[0][f].v;
		//planeBoxCheck[f].argb = MAKE_ARGB(255,255,255,255);
	}

	/*
	planeBoxCheck[0].p =
		Vector3(gGame.m_statePlaying.getplanexpos() - 5.0f,
		gGame.m_statePlaying.getplaneypos() + 5.0f,
		gGame.m_statePlaying.getplanezpos());
	planeBoxCheck[1].p =
		Vector3(gGame.m_statePlaying.getplanexpos() + 5.0f,
		gGame.m_statePlaying.getplaneypos() + 5.0f,
		gGame.m_statePlaying.getplanezpos());
	planeBoxCheck[2].p =
		Vector3(gGame.m_statePlaying.getplanexpos() + 5.0f,
		gGame.m_statePlaying.getplaneypos() - 5.0f,
		gGame.m_statePlaying.getplanezpos());
	planeBoxCheck[3].p =
		Vector3(gGame.m_statePlaying.getplanexpos() - 5.0f,
		gGame.m_statePlaying.getplaneypos() - 5.0f,
		gGame.m_statePlaying.getplanezpos());


	gRenderer.selectTexture(m_nGridProgGoalTexture);
	gRenderer.renderQuad(planeBoxCheck);

	*/
	
	float relPlaneX = 0.0f;
	float relPlaneY = 0.0f;

	for(int k=0; k<4; k++){
	
			if(zPosSuccessStart[k] == 0.0f){
				zPosSuccessStart[k]
					= (gGame.m_statePlaying.getNodePos(1, k) * 60.0f);
			}
			
			for(int nodeI=0; nodeI<gGame.m_statePlaying.getNodeTotal(k); nodeI++){
				float zNode = gGame.m_statePlaying.getNodePos(nodeI, k) * 60.0f;
				zNode -= 8.0f;
				nodeVerts[0][0].p = nodeVerts[3][3].p = nodeVerts[4][1].p
					= Vector3(0 - 2.0f, 360.0f + 2.0f, zNode - 2.0f);
				nodeVerts[0][1].p = nodeVerts[2][0].p = nodeVerts[3][2].p
					= Vector3(0 + 2.0f, 360.0f + 2.0f, zNode - 2.0f);
				nodeVerts[0][2].p = nodeVerts[1][1].p = nodeVerts[2][3].p
					= Vector3(0 + 2.0f, 360.0f - 2.0f, zNode - 2.0f);
				nodeVerts[0][3].p = nodeVerts[1][0].p = nodeVerts[4][2].p
					= Vector3(0 - 2.0f, 360.0f - 2.0f, zNode - 2.0f);
				nodeVerts[5][1].p = nodeVerts[3][0].p = nodeVerts[4][0].p
					= Vector3(0 - 2.0f, 360.0f + 2.0f, zNode + 2.0f);
				nodeVerts[5][0].p = nodeVerts[2][1].p = nodeVerts[3][1].p
					= Vector3(0 + 2.0f, 360.0f + 2.0f, zNode + 2.0f);
				nodeVerts[5][3].p = nodeVerts[1][2].p = nodeVerts[2][2].p
					= Vector3(0 + 2.0f, 360.0f - 2.0f, zNode + 2.0f);
				nodeVerts[5][2].p = nodeVerts[1][3].p = nodeVerts[4][3].p
					= Vector3(0 - 2.0f, 360.0f - 2.0f, zNode + 2.0f);


				//if lane stick mode
				//Vector3 xTrans = Vector3(18,0,0);
				//Vector3 yTrans = Vector3(0,18,0);


				for(int l=0; l<6; l++){
					for(int o=0; o<4; o++){
					
						//if lane stick mode
						/*switch(k){
						case 0:
							nodeVerts[l][o].p -= yTrans;
							break;
						case 1:
							nodeVerts[l][o].p += xTrans;
							break;
						case 2:
							nodeVerts[l][o].p += yTrans;
							break;
						case 3:
							nodeVerts[l][o].p -= xTrans;
							break;
						default:
							break;
						}*/


						//if lane stick mode
						/*if(k==0){
							nodeVerts[l][o].p.x
								+= ((gGame.m_statePlaying.m_mtp[k]->getPitchRatio(nodeI))
								*4.0f);
							nodeVerts[l][o].p.y
								+= ((gGame.m_statePlaying.m_mtp[k]->getRYTrans(nodeI))
								*3.0f);
						}
						else if(k==2){
							nodeVerts[l][o].p.x
								-= ((gGame.m_statePlaying.m_mtp[k]->getPitchRatio(nodeI))
								*4.0f);
							nodeVerts[l][o].p.y
								-= ((gGame.m_statePlaying.m_mtp[k]->getRYTrans(nodeI))
								*3.0f);
						}
						else if(k==1){
							nodeVerts[l][o].p.y
								+= ((gGame.m_statePlaying.m_mtp[k]->getPitchRatio(nodeI))
								*3.0f);
							nodeVerts[l][o].p.x
								+= ((gGame.m_statePlaying.m_mtp[k]->getRXTrans(nodeI))
								*3.0f);
						}
						else if(k==3){
							nodeVerts[l][o].p.y
								-= ((gGame.m_statePlaying.m_mtp[k]->getPitchRatio(nodeI))
								*4.0f);
							nodeVerts[l][o].p.x
								-= ((gGame.m_statePlaying.m_mtp[k]->getRXTrans(nodeI))
								*2.0f);
						}*/


						if(k==0){
							nodeVerts[l][o].p.x
								+= ((gGame.m_statePlaying.m_mtp[k]->getRXPos(nodeI))
								* m_fGridWidth);
							nodeVerts[l][o].p.y
								+= ((gGame.m_statePlaying.m_mtp[k]->getRYPos(nodeI))
								* m_fGridWidth);
						}
						else if(k==2){
							nodeVerts[l][o].p.x
								+= ((gGame.m_statePlaying.m_mtp[k]->getRXPos(nodeI))
								* (-1.0f) * m_fGridWidth);
							nodeVerts[l][o].p.y
								+= ((gGame.m_statePlaying.m_mtp[k]->getRYPos(nodeI))
								* (-1.0f) * m_fGridWidth);
						}
						else if(k==1){
							nodeVerts[l][o].p.x
								+= ((gGame.m_statePlaying.m_mtp[k]->getRYPos(nodeI))
								* (-1.0f) * m_fGridWidth);
							nodeVerts[l][o].p.y
								+= ((gGame.m_statePlaying.m_mtp[k]->getRXPos(nodeI))
								* m_fGridWidth);
						}
						else if(k==3){
							nodeVerts[l][o].p.x
								+= ((gGame.m_statePlaying.m_mtp[k]->getRYPos(nodeI))
								* m_fGridWidth);
							nodeVerts[l][o].p.y
								+= ((gGame.m_statePlaying.m_mtp[k]->getRXPos(nodeI))
								* (-1.0f) * m_fGridWidth);
						}


					}


					if(getGridActive(k)){
							if(zNode > zPosActiveStart[k] && zNode < zPosActiveEnd[k]){

								float zNodeFake = ((zNode - zPosActiveStart[k]) / 4.0f)
									+ zPosActiveStart[k];
								
								float zaDist = 0.0f;

								if(gGame.m_statePlaying.getplanezpos() > zNodeFake){
									zaDist 
										= gGame.m_statePlaying.getplanezpos() - zNodeFake;
								}
								
								Vector3 zaTrans[4];
								zaTrans[0] = Vector3(0, -zaDist, 0);
								zaTrans[1] = Vector3(zaDist, 0, 0);
								zaTrans[2] = Vector3(0, zaDist, 0);
								zaTrans[3] = Vector3(-zaDist, 0, 0);
								for(int zb=0;zb<4;zb++){
									nodeVerts[l][zb].p += zaTrans[k];
								}

							}
						}

					relPlaneX = gGame.m_statePlaying.getplanexpos(); 
					relPlaneY = gGame.m_statePlaying.getplaneypos();

					//skewed hitbox correction
					relPlaneY -= 2.0f;

					float relPlaneTrans = 0.0f;
					float hitBoxRad = 5.5f;

					switch(gGame.m_statePlaying.getCurrentLaneFlag()){
					case 0:
						relPlaneY += relPlaneTrans;
						break;
					case 1:
						relPlaneX -= relPlaneTrans;
						break;
					case 2:
						relPlaneY -= relPlaneTrans;
						break;
					case 3:
						relPlaneX += relPlaneTrans;
						break;
					default:
						break;
					}

					if(zNode > gGame.m_statePlaying.getplanezpos()){
						/*
						switch(k){
						case 0:
							for(int a = 0; a < 4; a++) nodeVerts[l][a].argb = MAKE_ARGB(255,150,150,100);
							break;
						case 1:
							for(int a = 0; a < 4; a++) nodeVerts[l][a].argb = MAKE_ARGB(255,100,200,100);
							break;
						case 2:
							for(int a = 0; a < 4; a++) nodeVerts[l][a].argb = MAKE_ARGB(255,100,100,200);
							break;
						case 3:
							for(int a = 0; a < 4; a++) nodeVerts[l][a].argb = MAKE_ARGB(255,200,100,100);
							break;
						default:
							break;
						}
						*/

						

						if(zNode - gGame.m_statePlaying.getplanezpos() < 2.0f){
							if(getGridActive(k)){
								if(gGame.m_statePlaying.m_mtp[k]->getCollected(nodeI) != 1){
									gGame.m_statePlaying.m_mtp[k]->setCollected(nodeI, 1);
									gGame.m_statePlaying.adjustXSoundVolumeByActiveLane(k, true);
									addScore();
									prevNode[k] = nodeI;
									zPosSuccessStart[k]
									= (gGame.m_statePlaying.getNodePos(nodeI+1, k) * 60.0f);
								}
							}
							else if(
								(relPlaneX + hitBoxRad > nodeVerts[0][0].p.x
								&& relPlaneX - hitBoxRad < nodeVerts[0][1].p.x)
								&& (relPlaneY - hitBoxRad < nodeVerts[0][2].p.y
								&& relPlaneY + hitBoxRad > nodeVerts[0][3].p.y)){
									if(gGame.m_statePlaying.m_mtp[k]->getCollected(nodeI) != 1){
										if(gGame.m_statePlaying.getCurrentLaneFlag() == k){
											gGame.m_statePlaying.m_mtp[k]->setCollected(nodeI, 1);
											gGame.m_statePlaying.adjustXSoundVolumeByActiveLane(k, true);
											addScore();
											prevNode[k] = nodeI;
										}
									}
							}
						}
						for(int a = 0; a < 4; a++)
							nodeVerts[l][a].argb
								= MAKE_ARGB(laneColor[k].alpha,
								(int)((float)laneColor[k].red * 1.6f) + 92,
								(int)((float)laneColor[k].green * 1.6f) + 92,
								(int)((float)laneColor[k].blue * 1.6f) + 92);
					}
					else{
						if(gGame.m_statePlaying.getplanezpos() - zNode < 2.0f){
							if(getGridActive(k)){
								if(gGame.m_statePlaying.m_mtp[k]->getCollected(nodeI) != 1){
									gGame.m_statePlaying.m_mtp[k]->setCollected(nodeI, 1);
									gGame.m_statePlaying.adjustXSoundVolumeByActiveLane(k, true);
									addScore();
									prevNode[k] = nodeI;
									zPosSuccessStart[k]
									= (gGame.m_statePlaying.getNodePos(nodeI+1, k) * 60.0f);
								}
							}
							else if(
								(relPlaneX + hitBoxRad > nodeVerts[0][0].p.x
								&& relPlaneX - hitBoxRad < nodeVerts[0][1].p.x)
								&& (relPlaneY - hitBoxRad < nodeVerts[0][2].p.y
								&& relPlaneY + hitBoxRad > nodeVerts[0][3].p.y)){
									if(gGame.m_statePlaying.m_mtp[k]->getCollected(nodeI) != 1){
										if(gGame.m_statePlaying.getCurrentLaneFlag() == k){
											gGame.m_statePlaying.m_mtp[k]->setCollected(nodeI, 1);
											gGame.m_statePlaying.adjustXSoundVolumeByActiveLane(k, true);
											addScore();
											prevNode[k] = nodeI;
										}
									}
							}
							for(int a = 0; a < 4; a++)
							nodeVerts[l][a].argb
								= MAKE_ARGB(laneColor[k].alpha,
								(int)((float)laneColor[k].red * 1.8f) + 76,
								(int)((float)laneColor[k].green * 1.8f) + 76,
								(int)((float)laneColor[k].blue * 1.8f) + 76);
						}
						else{
							if(gGame.m_statePlaying.m_mtp[k]->getCollected(nodeI) == 0){
								gGame.m_statePlaying.m_mtp[k]->setCollected(nodeI, 2);
								gGame.m_statePlaying.adjustXSoundVolumeByActiveLane(k, false);
								prevNode[k] = nodeI;
								zPosSuccessStart[k]
									= (gGame.m_statePlaying.getNodePos(nodeI+1, k) * 60.0f);
							}
							float passedDistance = gGame.m_statePlaying.getplanezpos() - zNode;
							Vector3 passedTrans[4];
							passedTrans[0] = Vector3(0, -passedDistance, 0);
							passedTrans[1] = Vector3(passedDistance, 0, 0);
							passedTrans[2] = Vector3(0, passedDistance, 0);
							passedTrans[3] = Vector3(-passedDistance, 0, 0);
							for(int a = 0; a < 4; a++){
								if(gGame.m_statePlaying.m_mtp[k]->getCollected(nodeI) == 2){
									nodeVerts[l][a].argb = MAKE_ARGB(255,100,100,100);
								}
								else{
									for(int b = 0; b < 4; b++){
									nodeVerts[l][b].argb
										= MAKE_ARGB(laneColor[k].alpha,
										(int)((float)laneColor[k].red * 2.75f),
										(int)((float)laneColor[k].green * 2.75f),
										(int)((float)laneColor[k].blue * 2.75f));
									}
									
										nodeVerts[l][a].p += passedTrans[k];
									
								}
							
							}
						}
					}
				}

				
				Vector3 shadowTrans[4];
				float sTransExt = 4 + m_fGridWidth;
				shadowTrans[0] = Vector3(0,((-1.0f)*sTransExt)
					+ gGame.m_statePlaying.getPlaneOrigSpawnVal().y,0);
				shadowTrans[1] = Vector3(sTransExt,0,0);
				shadowTrans[2] = Vector3(0,sTransExt 
					+ gGame.m_statePlaying.getPlaneOrigSpawnVal().y,0);
				shadowTrans[3] = Vector3((-1.0f)*sTransExt,0,0);

				Vector3 shadowMult[2];
				shadowMult[0] = Vector3(1.0f,0,1.0f);
				shadowMult[1] = Vector3(0,1.0f,1.0f);

				for(int c=0; c<4; c++){
					nodeShadowVerts[c].p = Vector3(
						nodeVerts[((k+2) % 4) + 1][c].p.x	* shadowMult[k%2].x,
						nodeVerts[((k+2) % 4) + 1][c].p.y	* shadowMult[k%2].y,
						nodeVerts[((k+2) % 4) + 1][c].p.z	* shadowMult[k%2].z)
						+ shadowTrans[k];
					nodeShadowVerts[c].argb = nodeVerts[0][c].argb;
				}

				float sExt = 1.2f;

				if(k==0){
					nodeShadowVerts[0].p += Vector3(-sExt,0,sExt);
					nodeShadowVerts[1].p += Vector3(sExt,0,sExt);
					nodeShadowVerts[2].p += Vector3(sExt,0,-sExt);
					nodeShadowVerts[3].p += Vector3(-sExt,0,-sExt);
				}
				else if(k==1){
					nodeShadowVerts[0].p += Vector3(0,-sExt,sExt);
					nodeShadowVerts[1].p += Vector3(0,sExt,sExt);
					nodeShadowVerts[2].p += Vector3(0,sExt,-sExt);
					nodeShadowVerts[3].p += Vector3(0,-sExt,-sExt);
				}
				else if(k==2){
					nodeShadowVerts[0].p += Vector3(-sExt,0,-sExt);
					nodeShadowVerts[1].p += Vector3(sExt,0,-sExt);
					nodeShadowVerts[2].p += Vector3(sExt,0,sExt);
					nodeShadowVerts[3].p += Vector3(-sExt,0,sExt);
				}
				else if(k==3){
					nodeShadowVerts[0].p += Vector3(0,sExt,sExt);
					nodeShadowVerts[1].p += Vector3(0,-sExt,sExt);
					nodeShadowVerts[2].p += Vector3(0,-sExt,-sExt);
					nodeShadowVerts[3].p += Vector3(0,sExt,-sExt);
				}




				

				gRenderer.selectTexture(m_nGridShadowTexture);
				gRenderer.renderQuad(nodeShadowVerts);

				Vector3 rPivot = nodeVerts[0][0].p + Vector3(2,-2,2);
				Vector3 rPoint;
				Vector3 rDir;

				for(int rb=0;rb<6;rb++){
					for(int rc=0;rc<4;rc++){
						rPoint = nodeVerts[rb][rc].p;
						rDir = rPoint - rPivot;
						tve.x = rDir.x;
						tve.y = rDir.y;
						tve.z = rDir.z;
						float kPi = 3.14159265359f;
						float rotScalar
							= ((float)((gGame.getTimeMeasure() % 3) * 4)
							+ (float)gGame.getTimeBeat()
							+ gGame.getBeatFrac()) / 12.0f;
						float nodeRotScalar
							= ((float)(((gGame.m_statePlaying.m_mtp[k]->getMeasureValue(nodeI) - 1 )% 3) * 4)
							+ (float)(gGame.m_statePlaying.m_mtp[k]->getBeatValue(nodeI) - 1)
							+ gGame.m_statePlaying.m_mtp[k]->getRatioValue(nodeI)) / 12.0f;
						rotScalar = rotScalar + nodeRotScalar;
						D3DXMatrixRotationYawPitchRoll(&mve,
							rotScalar * 4.0f * kPi,
							rotScalar * 2.0f * kPi,
							rotScalar * 1.0f * kPi);
						D3DXVec3Transform(&pve,&tve,&mve);
						rPoint.x = pve.x;
						rPoint.y = pve.y;
						rPoint.z = pve.z;
						rPoint = rPoint + rPivot;

						nodeVerts[rb][rc].p = rPoint;
					}
				}

				if(gGame.m_statePlaying.getCurrentLaneFlag() == k){
					gRenderer.selectTexture(m_nGridActiveTexture);
					for(int m=0; m<6; m++){
						gRenderer.renderQuad(nodeVerts[m]);
					}
				}
				
		}

		if(!(getGridActive(k))){
			if(gGame.m_statePlaying.getplanezpos()
				> (zPosSuccessStart[k] + (60.0f * numBeatsToActivateLane))){
					activateGrid(k,(4*numBeatsToActivateLane));
					zPosActiveStart[k] = gGame.m_statePlaying.getplanezpos();
					zPosActiveEnd[k] = m_gridActive[k].gridActiveVerts[0].z;
			}
		}

	}

	RenderVertexL planeShadowVerts[4];

	planeShadowVerts[0].u = 0.0f; planeShadowVerts[0].v = 0.0f;
  planeShadowVerts[1].u = 1.0f; planeShadowVerts[1].v = 0.0f;
  planeShadowVerts[2].u = 1.0f; planeShadowVerts[2].v = 1.0f;
  planeShadowVerts[3].u = 0.0f; planeShadowVerts[3].v = 1.0f;

	float pTransExt = 4 + m_fGridWidth;

	for(int g=0; g<4; g++){
		planeShadowVerts[g].p = Vector3(
			gGame.m_statePlaying.getplanexpos(),
			gGame.m_statePlaying.getplaneypos(),
			gGame.m_statePlaying.getplanezpos());
	}

	float pExt = 2.0f + 1.0f;

	if(gGame.m_statePlaying.getCurrentLaneFlag()==0){
		planeShadowVerts[0].p += Vector3(-pExt,0,pExt);
		planeShadowVerts[1].p += Vector3(pExt,0,pExt);
		planeShadowVerts[2].p += Vector3(pExt,0,-pExt);
		planeShadowVerts[3].p += Vector3(-pExt,0,-pExt);
		for(int ga=0;ga<4;ga++){
			planeShadowVerts[ga].p.y
				= gGame.m_statePlaying.getPlaneOrigSpawnVal().y - pTransExt;
		}
	}
	else if(gGame.m_statePlaying.getCurrentLaneFlag()==1){
		planeShadowVerts[0].p += Vector3(0,-pExt,pExt);
		planeShadowVerts[1].p += Vector3(0,pExt,pExt);
		planeShadowVerts[2].p += Vector3(0,pExt,-pExt);
		planeShadowVerts[3].p += Vector3(0,-pExt,-pExt);
		for(int gb=0;gb<4;gb++){
			planeShadowVerts[gb].p.x = 0.0f + pTransExt;
		}
	}
	else if(gGame.m_statePlaying.getCurrentLaneFlag()==2){
		planeShadowVerts[0].p += Vector3(pExt,0,pExt);
		planeShadowVerts[1].p += Vector3(-pExt,0,pExt);
		planeShadowVerts[2].p += Vector3(-pExt,0,-pExt);
		planeShadowVerts[3].p += Vector3(pExt,0,-pExt);
		for(int gc=0;gc<4;gc++){
			planeShadowVerts[gc].p.y
				= gGame.m_statePlaying.getPlaneOrigSpawnVal().y + pTransExt;
		}
	}
	else if(gGame.m_statePlaying.getCurrentLaneFlag()==3){
		planeShadowVerts[0].p += Vector3(0,pExt,pExt);
		planeShadowVerts[1].p += Vector3(0,-pExt,pExt);
		planeShadowVerts[2].p += Vector3(0,-pExt,-pExt);
		planeShadowVerts[3].p += Vector3(0,pExt,-pExt);
		for(int gd=0;gd<4;gd++){
			planeShadowVerts[gd].p.x = 0.0f - pTransExt;
		}
	}

	

	for(int ge=0;ge<4;ge++){
		planeShadowVerts[ge].argb = MAKE_ARGB(255,200,200,200);
	}

	gRenderer.selectTexture(m_nGridShadowTexture);
	gRenderer.renderQuad(planeShadowVerts);

}

void GridManager::renderActiveGrids() {

	for(int i=0; i<4; i++){
    if(m_gridActive[i].isActive == true){
			
			RenderVertexL tmpVerts[4];
			tmpVerts[0].u = 0.0f; tmpVerts[0].v = 0.0f;
			tmpVerts[1].u = 1.0f; tmpVerts[1].v = 0.0f;
			tmpVerts[2].u = 1.0f; tmpVerts[2].v = 1.0f;
			tmpVerts[3].u = 0.0f; tmpVerts[3].v = 1.0f;

			tmpVerts[0].p = m_gridActive[i].gridActiveVerts[0];
			tmpVerts[1].p = m_gridActive[i].gridActiveVerts[1];
			tmpVerts[2].p = m_gridActive[i].gridActiveVerts[2];
			tmpVerts[3].p = m_gridActive[i].gridActiveVerts[3];

			tmpVerts[0].p.z = tmpVerts[1].p.z
				= tmpVerts[2].p.z
				+ (gGame.m_statePlaying.getplanezpos() - tmpVerts[2].p.z) * 4.0f;

			if(tmpVerts[0].p.z > m_gridActive[i].gridActiveVerts[0].z){
				tmpVerts[0].p.z = tmpVerts[1].p.z
					= m_gridActive[i].gridActiveVerts[0].z;
			}

			tmpVerts[2].p.z -= 60.0f;
			tmpVerts[3].p.z -= 60.0f;
			
			tmpVerts[0].argb = tmpVerts[1].argb 
				= tmpVerts[2].argb = tmpVerts[3].argb 
				= MAKE_ARGB(laneColor[i].alpha,laneColor[i].red,
				laneColor[i].green,laneColor[i].blue);
			
      gRenderer.selectTexture(m_nGridActiveTexture);
			
			gRenderer.renderQuad(tmpVerts);

    }
	}
	
}

void GridManager::renderGridEdges() {

  gRenderer.selectTexture(m_nGridSideTexture);
  gRenderer.renderQuad(m_vBottomRender);

	gRenderer.selectTexture(m_nGridSideTexture);
  gRenderer.renderQuad(m_vTopRender);

	gRenderer.selectTexture(m_nGridSideTexture);
  gRenderer.renderQuad(m_vLeftRender);

	gRenderer.selectTexture(m_nGridSideTexture);
  gRenderer.renderQuad(m_vRightRender);
}

void GridManager::renderGridBeats(float p_fPozZ) {
  
  int beat = gGame.getTimeBeat(), measure = gGame.getTimeMeasure();

  float beatforwardoffset = 60.0f;
	int trueMeasure = ((measure - 1) * 4) + beat;   // Needed to make it (measure - 1) since measure now starts at '1'
  float frontBeatOffset = beatforwardoffset * float(trueMeasure + 1);
  float backBeatOffset = beatforwardoffset * float(trueMeasure);

  float t_fBackOffset, t_fFrontOffset;

	for(int i=0; i<4; i++) {
		
    t_fBackOffset = backBeatOffset + (beatforwardoffset * float(i));
    t_fFrontOffset = frontBeatOffset + (beatforwardoffset * float(i));

    m_vBeats[i][0][0].p.z = t_fFrontOffset;
		m_vBeats[i][0][1].p.z = t_fFrontOffset;
		m_vBeats[i][0][2].p.z = t_fBackOffset;
		m_vBeats[i][0][3].p.z = t_fBackOffset;

		m_vBeats[i][1][0].p.z = t_fFrontOffset;
		m_vBeats[i][1][1].p.z = t_fFrontOffset;
		m_vBeats[i][1][2].p.z = t_fBackOffset;
		m_vBeats[i][1][3].p.z = t_fBackOffset;

		m_vBeats[i][2][0].p.z = t_fFrontOffset;
		m_vBeats[i][2][1].p.z = t_fFrontOffset;
		m_vBeats[i][2][2].p.z = t_fBackOffset;
		m_vBeats[i][2][3].p.z = t_fBackOffset;

		m_vBeats[i][3][0].p.z = t_fFrontOffset;
		m_vBeats[i][3][1].p.z = t_fFrontOffset;
		m_vBeats[i][3][2].p.z = t_fBackOffset;
    m_vBeats[i][3][3].p.z = t_fBackOffset;
		
	}
	
	RenderVertexL t_Verts[4];
	float pulseWidth = 0.0f;

	for(int i=0; i<4; i++){
		for(int j=0; j<4; j++){
      t_Verts[0].p = m_vBeats[i][j][0].p;
			t_Verts[0].u = m_vBeats[i][j][0].u;
			t_Verts[0].v = m_vBeats[i][j][0].v;
			t_Verts[1].p = m_vBeats[i][j][1].p;
			t_Verts[1].u = m_vBeats[i][j][1].u;
			t_Verts[1].v = m_vBeats[i][j][1].v;
			t_Verts[2].p = m_vBeats[i][j][2].p;
			t_Verts[2].u = m_vBeats[i][j][2].u;
			t_Verts[2].v = m_vBeats[i][j][2].v;
			t_Verts[3].p = m_vBeats[i][j][3].p;
			t_Verts[3].u = m_vBeats[i][j][3].u;
			t_Verts[3].v = m_vBeats[i][j][3].v;

      if(t_Verts[3].p.z < p_fPozZ + pulseWidth){
				for (int a = 0; a < 4; a++) t_Verts[a].argb = MAKE_ARGB(255,60,255,120);
			}
			else{
				for (int a = 0; a < 4; a++) t_Verts[a].argb = MAKE_ARGB(255,255,255,255);
			}
      gRenderer.selectTexture(m_nGridBeatTexture);
			gRenderer.renderQuad(t_Verts);
		}
	}

}

void GridManager::checkActiveGrids(float p_fPosZ) {

	float planeZ = p_fPosZ;
	for(int i=0; i<4; i++) {
		if(m_gridActive[i].isActive) {
			if(planeZ > m_gridActive[i].gridActiveVerts[0].z) {
				m_gridActive[i].isActive = false;
			}
		}
	}

}

int GridManager::activateGrid(int lane, int numBeats) {

	//lanes are 0 1 2 3 starting from bottom going counter clockwise
	if(m_gridActive[lane].isActive){
		return -1; //lane is already active
	}

	/*
	float startSig = gGame.getBeatFrac()
		+ (float)(gGame.getTimeBeat())
		+ (float)((gGame.getTimeMeasure()) * 4);
		*/
	float startSig = gGame.m_statePlaying.getplanezpos();

	float endSig = startSig + ((float)numBeats * 60.0f);

	

	m_gridActive[lane].gridActiveVerts[2].z = m_gridActive[lane].gridActiveVerts[3].z 
		= m_vPlaneSpawn.z + startSig;
	m_gridActive[lane].gridActiveVerts[0].z = m_gridActive[lane].gridActiveVerts[1].z 
		= m_vPlaneSpawn.z + endSig;

	//float beatFracOffset = 1.0f - gGame.getBeatFrac();
	//int currTimeOffset = ((gGame.getTimeMeasure() - 1) * 4) + (gGame.getTimeBeat() - 1);
 // float currSpaceOffset = m_vPlaneSpawn.z + (m_fGridFrontZOffset * ((float)(currTimeOffset) + 1.0f));

	////m_gridActive[lane].gridActiveVerts[2].z = m_gridActive[lane].gridActiveVerts[3].z = getPosition().z + (beatForwardOffset * beatFracOffset);
	//m_gridActive[lane].gridActiveVerts[2].z = m_gridActive[lane].gridActiveVerts[3].z = currSpaceOffset;
	//m_gridActive[lane].gridActiveVerts[0].z = m_gridActive[lane].gridActiveVerts[1].z 
	//	= m_gridActive[lane].gridActiveVerts[2].z + (m_fGridFrontZOffset * 4.0f * (float)(numMeasures));
	m_gridActive[lane].isActive = true;

	return 0;
}

int GridManager::deActivateGrid(int lane) {

	if(!m_gridActive[lane].isActive)
		return -1;

	m_gridActive[lane].isActive = false;
	return 0;

}

int GridManager::renderProgressGrid() {

	int laneFlag = gGame.m_statePlaying.getCurrentLaneFlag();
	

	RenderVertexL progVerts[4];
	progVerts[0].u = 0.0f; progVerts[0].v = 0.0f;
	progVerts[1].u = 1.0f; progVerts[1].v = 0.0f;
	progVerts[2].u = 1.0f; progVerts[2].v = 1.0f;
	progVerts[3].u = 0.0f; progVerts[3].v = 1.0f;

	float sh = 0.25f;

	Vector3 xTrans = Vector3(sh,0,0);
	Vector3 yTrans = Vector3(0,sh,0);

	/*
	for(int i=0; i<4; i++){
		progVerts[i].p = m_gridActive[laneFlag].gridActiveVerts[i];
	}

	progVerts[0].p.z = progVerts[1].p.z
		= gGame.m_statePlaying.getplanezpos();
	progVerts[2].p.z = progVerts[3].p.z
		= progVerts[0].p.z - 180.0f;
		*/
	
	for(int i=0; i<4; i++){
		progVerts[i].p = m_gridActive[laneFlag].gridActiveVerts[i];
		if(laneFlag==0){
			progVerts[i].p += yTrans;
		}
		else if(laneFlag==1){
			progVerts[i].p -= xTrans;
		}
		else if(laneFlag==2){
			progVerts[i].p -= yTrans;
		}
		else if(laneFlag==3){
			progVerts[i].p += xTrans;
		}
	}
	if(getGridActive(laneFlag)){
		progVerts[2].p.z = progVerts[3].p.z
			= gGame.m_statePlaying.getplanezpos() - 180.0f;
		progVerts[0].p.z = progVerts[1].p.z
			= zPosActiveStart[laneFlag] + ((60.0f * numBeatsToActivateLane) / 4.0f);
	}
	else{
		float progDist
			= (gGame.m_statePlaying.getplanezpos() - zPosSuccessStart[laneFlag]);

		progVerts[2].p.z = progVerts[3].p.z
			= gGame.m_statePlaying.getplanezpos() - 180.0f;
		progVerts[0].p.z = progVerts[1].p.z
			= gGame.m_statePlaying.getplanezpos() + (progDist / 4.0f);

	}
	
	/*
	float progScale
		= (gGame.m_statePlaying.getplanezpos() - zPosSuccessStart[laneFlag])
		/ (60.0f * (float)numBeatsToActivateLane);
		

	Vector3 progTransX = Vector3((1.0f - progScale) * m_fGridWidth,0,0);
	Vector3 progTransY = Vector3(0,(1.0f - progScale) * m_fGridWidth,0);

	
	if(laneFlag==0){
		progVerts[1].p -= progTransX;
		progVerts[2].p -= progTransX;
		progVerts[0].p += progTransX;
		progVerts[3].p += progTransX;
	}
	else if(laneFlag==1){
		progVerts[1].p -= progTransY;
		progVerts[2].p -= progTransY;
		progVerts[0].p += progTransY;
		progVerts[3].p += progTransY;
	}
	else if(laneFlag==2){
		progVerts[1].p += progTransX;
		progVerts[2].p += progTransX;
		progVerts[0].p -= progTransX;
		progVerts[3].p -= progTransX;
	}
	else if(laneFlag==3){
		progVerts[1].p += progTransY;
		progVerts[2].p += progTransY;
		progVerts[0].p -= progTransY;
		progVerts[3].p -= progTransY;
	}
	


	for(int i=0; i<4; i++){
		if(laneFlag==0){
			progVerts[i].p += yTrans;
			progVerts[i].p.x += gGame.m_statePlaying.getplanexpos();
			progVerts[0].p.x = progVerts[1].p.x
				= gGame.m_statePlaying.getplanexpos();
		}
		else if(laneFlag==1){
			progVerts[i].p -= xTrans;
			progVerts[i].p.y 
				+= (gGame.m_statePlaying.getplaneypos() - m_vPlaneSpawn.y);
			progVerts[0].p.y = progVerts[1].p.y
				= (gGame.m_statePlaying.getplaneypos() - m_vPlaneSpawn.y);
		}
		else if(laneFlag==2){
			progVerts[i].p -= yTrans;
			progVerts[i].p.x += gGame.m_statePlaying.getplanexpos();
			progVerts[0].p.x = progVerts[1].p.x
				= gGame.m_statePlaying.getplanexpos();
		}
		else if(laneFlag==3){
			progVerts[i].p += xTrans;
			progVerts[i].p.x 
				+= (gGame.m_statePlaying.getplaneypos() - m_vPlaneSpawn.y);
			progVerts[0].p.y = progVerts[1].p.y
				= (gGame.m_statePlaying.getplaneypos() - m_vPlaneSpawn.y);
		}
	}
	*/

	progVerts[0].argb = progVerts[1].argb 
		= progVerts[2].argb = progVerts[3].argb 
		= MAKE_ARGB(laneColor[laneFlag].alpha,laneColor[laneFlag].red,
		laneColor[laneFlag].green,laneColor[laneFlag].blue);
	
    gRenderer.selectTexture(m_nGridProgressTexture);
	
		gRenderer.renderQuad(progVerts);


		if(getGridActive(laneFlag)){
			progVerts[0].p.z = progVerts[1].p.z
				= gGame.m_statePlaying.getplanezpos()
				+ ((60.0f * numBeatsToActivateLane) / 4.0f);
		}

		gRenderer.selectTexture(m_nGridProgGoalTexture);
	
		gRenderer.renderQuad(progVerts);
		


		return 0;
}

void GridManager::updateColors() {
	int beatColorDuration = 128;
	int colorMax = 80;
	int tempHue 
		= ((int)(((gGame.getBeatFrac() 
		+ ((float)((((gGame.getTimeMeasure() - 1) * 4)
		+ (gGame.getTimeBeat() - 1)) % beatColorDuration)))
		/ (float)(beatColorDuration)) * 360.0f)) % 360;
	for(int i=0; i<4; i++){
		laneHue[i] = (tempHue + (90 * i)) % 360;
		float cval = 1.0f;
		float xval = 1.0f -
			abs(((float)(laneHue[i] % 120) / 60.0f) - 1.0f );
		int cival = (int)(cval * colorMax);
		int xival = (int)(xval * colorMax);
		int hueSection = laneHue[i] / 60;
		switch(hueSection){
		case 0:
			laneColor[i].red = cival;
			laneColor[i].green = xival;
			laneColor[i].blue = 0;
			break;
		case 1:
			laneColor[i].red = xival;
			laneColor[i].green = cival;
			laneColor[i].blue = 0;
			break;
		case 2:
			laneColor[i].red = 0;
			laneColor[i].green = cival;
			laneColor[i].blue = xival;
			break;
		case 3:
			laneColor[i].red = 0;
			laneColor[i].green = xival;
			laneColor[i].blue = cival;
			break;
		case 4:
			laneColor[i].red = xival;
			laneColor[i].green = 0;
			laneColor[i].blue = cival;
			break;
		case 5:
			laneColor[i].red = cival;
			laneColor[i].green = 0;
			laneColor[i].blue = xival;
			break;
		default:
			break;
		}
	}
}

int GridManager::getLaneColorRed(int lane){
	return laneColor[lane].red;
}

int GridManager::getLaneColorGreen(int lane){
	return laneColor[lane].green;
}

int GridManager::getLaneColorBlue(int lane){
	return laneColor[lane].blue;
}

int GridManager::getLaneHue(int lane){
	return laneHue[lane];
}

bool GridManager::getGridActive(int lane){
	return m_gridActive[lane].isActive;
}

int GridManager::addScore(){
	score += (10 * scoreMultiplier);
	return 0;
}

int GridManager::updateMultiplier(){
	int tmpMult = 0;
	for(int i=0;i<4;i++){
		if(getGridActive(i)){
			tmpMult++;
		}
	}
	scoreMultiplier = (int)pow(2.0f, tmpMult);
	return 0;
}