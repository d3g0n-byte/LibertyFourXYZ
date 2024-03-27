#include "rage_datResource.h"
#include "rage_crSkeleton.h"
#include "rage_crBone.h"
#include "resource_reader.h"
#include "memory_manager.h"
#include "rage_rmcDrawable.h"
#include "rage_grcTexture.h"
#include "rage_grmModel.h"
#include "rage_grmGeometry.h"
#include "rage_grcVertexBuffer.h"
#include "rage_grcIndexBuffer.h"
#include "rage_grcFvf.h"
#include "rage_light.h"

#include "rage_grmShader.h"

#include "rsc85_layout.h"

#include "utils.h"
#include "iv_pc_wdr.h"

using namespace rage;

namespace iv_pc_wdr {

	void processWdr(rage::ConstString& pszFilePath) {
		rage::ConstString pszResName = pszFilePath.getFileNameWithoutExt();
		rage::ConstString pszOutFile = rage::ConstString::format("%s/%s_new.wdr", pszFilePath.getFilePath(), pszFilePath.getFileNameWithoutExt());

		rage::datResource* pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("processWdr, pResource");
		pResource->m_pszDebugName = pszResName;
		rage::datResourceFileHeader* pResourceHeader = libertyFourXYZ::g_memory_manager.allocate<rage::datResourceFileHeader>("processWdr, pResourceHeader");
		libertyFourXYZ::readRsc85Resource((char*)pszFilePath.c_str(), pResourceHeader, pResource);

		//rage::crSkeletonData* pSkel = libertyFourXYZ::g_memory_manager.allocate<rage::crSkeletonData>();
		gtaDrawable* pDrawable = libertyFourXYZ::g_memory_manager.allocate<gtaDrawable>("processWdr, pDrawable");
		auto realPtr = pResource->getFixup((gtaDrawable*)(pResourceHeader->flags.getVBlockStart() + 0x50000000), sizeof(*pDrawable));

		libertyFourXYZ::g_memory_manager.release(pResourceHeader);

		copy_class<gtaDrawable>(pDrawable, realPtr);

		pDrawable->place(pResource);
		pDrawable->clearRefCount();

		libertyFourXYZ::g_memory_manager.release(pResource);


		/*
		for (DWORD i = 0; i < pDrawable->m_pSkeleton->m_wNumBones; i++) trace("%s", pDrawable->m_pSkeleton->m_pBones[i].m_pszName);
		for (DWORD i = 0; i < pDrawable->m_pShaderGroup->m_shaders.m_count; i++) {
			auto shader = pDrawable->m_pShaderGroup->m_shaders.m_pElements[i].get();
			trace("main info: version=%u, name=%s, preset=%s", (DWORD)shader->m_nbVersion, shader->m_pszName, shader->m_pszSps);
			trace("params:");
			for (DWORD j = 0; j < shader->m_effect.m_dwParameterCount; j++) {
				auto param = shader->m_effect.m_ppParameters + j;
				if (!param->pVec4) {
					trace("\tNULL");
				}
				else if (shader->m_effect.m_pParameterTypes[j] == 0) {
					if (param->txd.pTxdRef->m_nbResourceType == rage::eTextureResourceType::TEXTURE_REFERENCE)
						trace("\ttype=grcTextureReference, size=%u, name=%s", 0, param->txd.pTxdRef->m_pszName);
					else if (param->txd.pTxdRef->m_nbResourceType == rage::eTextureResourceType::TEXTURE) {
						auto txd = param->txd.pTxd;
						trace("\ttype=grcTexture, size=%u, name=%s, width=%u. height=%u, mips=%u, data_size=%u", 0, txd->m_pszName, (DWORD)txd->m_wWidth, (DWORD)txd->m_wHeight,
							(DWORD)txd->m_nbLevels, txd->getPixelDataSize());
					}
				}
				else {
					DWORD dwArraySize = 1;
					switch (shader->m_effect.m_pParameterTypes[i]) {
					case 8:
						dwArraySize = 6;
						break;
					case 1:
					case 4:
					case 14:
					case 15:
					case 16:
						dwArraySize = shader->m_effect.m_pParameterTypes[i];
						break;
					}
					trace("\ttype=Vector4_%u:", dwArraySize);
					for (BYTE n = 0; n < dwArraySize; n++) {
						rage::Vector4* pVec = param->pVec4 + n;
						trace("\t\t%.8f;%.8f;%.8f;%.8f", pVec->x, pVec->y, pVec->z, pVec->w);
					}
				}
			}
		}
		*/
		libertyFourXYZ::rsc85_layout* pLayout = libertyFourXYZ::g_memory_manager.allocate<libertyFourXYZ::rsc85_layout>("rebuild wdr v110, rsc85 layout");

		// optimize gfx buffer
		if (false){
			for (BYTE i = 0; i < 4; i++) {
				if (!pDrawable->m_lodgroup.m_lod.m_elements[i].pElement) continue;

				for (WORD j = 0; j < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models.m_count; j++) {
					for (WORD n = 0; n < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries.m_count; n++) {
						auto geom = pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries[n].get();

						for (BYTE o = 0; o < 4; o++) {
							if (!geom->m_vertexBuffers[o].get()) continue;

							auto vertBuf = geom->m_vertexBuffers[o].get();

							auto declaration = vertBuf->m_pDeclarations.get();

							rage::grcFvf oldDecl;
							memcpy(&oldDecl, declaration, sizeof * declaration);

							bool bChanged = 0;
							BYTE aChanged[18]{ 0 };
							for (BYTE l = 0; l < 18; l++) {
								int type = (declaration->m_fvfChannelSizes.qwVal >> (l * 4)) & 0xf;
								if (type == declaration->grcdsFloat2) {
									declaration->m_fvfChannelSizes.qwVal = setBits(declaration->m_fvfChannelSizes.qwVal, l * 4, declaration->grcdsHalf2, 4);
									bChanged = 1;
									aChanged[l] = 1;
								}
								else if (type == declaration->grcdsFloat3 || type == declaration->grcdsFloat4) {
									declaration->m_fvfChannelSizes.qwVal = setBits(declaration->m_fvfChannelSizes.qwVal, l * 4, declaration->grcdsHalf4, 4);
									bChanged = 1;
									aChanged[l] = 1;
								}
							}
							if (bChanged) {
								declaration->recomputeTotalSize();
								DWORD dwNewMemSize = declaration->m_nbFvfSize * vertBuf->m_wVertexCount;
								BYTE* pNewMem = libertyFourXYZ::g_memory_manager.allocate<BYTE>("processwdr, new verticles", dwNewMemSize);

								for (WORD l = 0; l < vertBuf->m_wVertexCount; l++) {
									DWORD dwPosInOldBuf = l * oldDecl.m_nbFvfSize;
									DWORD dwPosInNewBuf = l * declaration->m_nbFvfSize;
									for (BYTE k = 0; k < 18; k++) {
										if ((declaration->m_fvf.dwVal >> k) & 1) {
											if (!aChanged[k]) {
												DWORD dwElementSize = oldDecl.getSize((rage::grcFvf::grcFvfChannels)k);
												memcpy(pNewMem + dwPosInNewBuf, vertBuf->m_pVertexData.pElement + dwPosInOldBuf, dwElementSize);
												dwPosInOldBuf += dwElementSize;
												dwPosInNewBuf += dwElementSize;
											}
											else {
												int type = (oldDecl.m_fvfChannelSizes.qwVal >> (k * 4)) & 0xf;
												int newType = (declaration->m_fvfChannelSizes.qwVal >> (k * 4)) & 0xf;
												
												if (type >= declaration->grcdsFloat && type <= declaration->grcdsFloat4 &&
													newType >= declaration->grcdsHalf && newType <= declaration->grcdsHalf4) {
													rage::Vector4* pVec = (rage::Vector4*)(vertBuf->m_pVertexData.pElement + dwPosInOldBuf);
													rage::Half4 float16_4(*pVec);

													DWORD dwNewElementSize = rage::sm_TypeSizes[newType];
													memcpy(pNewMem + dwPosInNewBuf, &float16_4, dwNewElementSize);
													dwPosInOldBuf += rage::sm_TypeSizes[type];
													dwPosInNewBuf += dwNewElementSize;
												}
											}
										}
									}
								}
								geom->m_wStride = vertBuf->m_dwVertexSize = declaration->m_nbFvfSize;
								libertyFourXYZ::g_memory_manager.release(vertBuf->m_pVertexData.pElement);
								vertBuf->m_pVertexData.pElement = pNewMem;
							}
						}
					}
				}
			}
		}

		BYTE nbNewIndicesPerFace = 4;

		// wtf?
		if (false) {
			for (BYTE i = 0; i < 4; i++) {
				if (!pDrawable->m_lodgroup.m_lod.m_elements[i].pElement) continue;

				for (WORD j = 0; j < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models.m_count; j++) {
					for (WORD n = 0; n < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries.m_count; n++) {
						auto geom = pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries[n].get();
						
						BYTE nbOldIndicesPerFace = geom->m_nbIndicesPerFace;

						geom->m_dwFaceCount = geom->m_dwIndexCount / geom->m_nbIndicesPerFace;

						for (BYTE o = 0; o < 4; o++) {
							if (!geom->m_indexBuffers[o].get()) continue;

							auto idxBuf = geom->m_indexBuffers[o].get();

							WORD *pNewData = libertyFourXYZ::g_memory_manager.allocate<WORD>("process wdr, new idxData", idxBuf->m_dwIndexCount / nbOldIndicesPerFace * nbNewIndicesPerFace);

							DWORD dwOldPos = 0;
							DWORD dwNewPos = 0;

							WORD tmpBuf[0x8];
							for (DWORD g = 0; g < geom->m_dwFaceCount; g++) {
								for (BYTE f = 0; f < nbOldIndicesPerFace; f++)
									tmpBuf[f] = idxBuf->m_pIndexData[dwOldPos++];

								for (BYTE f = nbOldIndicesPerFace; f < nbNewIndicesPerFace; f++)
									tmpBuf[f] = tmpBuf[nbOldIndicesPerFace - 1];

								for (BYTE f = 0; f < nbNewIndicesPerFace; f++)
									pNewData[dwNewPos++] = tmpBuf[f];

							}

							geom->m_nbIndicesPerFace = nbNewIndicesPerFace;

							libertyFourXYZ::g_memory_manager.release(idxBuf->m_pIndexData.pElement);
							idxBuf->m_pIndexData.pElement = pNewData;

						}

					}
				}
			}
		}

		// shaders test
		if (false && pDrawable->m_pShaderGroup->m_shaders.m_count) {
			auto sgroup = pDrawable->m_pShaderGroup;
			for (auto &i : sgroup->m_shaders) {
				rage::grmShaderFx* pShader = i.get();
	
				if (pShader->bHavePreset) {
					pShader->bHavePreset = 0;
					pShader->m_pszPresetName.destroy();
				}
			}
		}

		// fvf to dynamic
		if (false) {
			for (BYTE i = 0; i < 4; i++) {
				if (!pDrawable->m_lodgroup.m_lod.m_elements[i].pElement) continue;

				for (WORD j = 0; j < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models.m_count; j++) {
					for (WORD n = 0; n < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries.m_count; n++) {
						auto geom = pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries[n].get();

						for (BYTE o = 0; o < 4; o++) {
							if (!geom->m_vertexBuffers[o].get()) continue;

							auto vertBuf = geom->m_vertexBuffers[o].get();

							auto declaration = vertBuf->m_pDeclarations.get();
							
							if (declaration->m_bDynamicOrder) continue;

							rage::grcFvf oldDecl;
							memcpy(&oldDecl, declaration, sizeof * declaration);

							for (BYTE k = 0; k < 14; k++) {
								rage::grcFvf::grcFvfChannels channel = (rage::grcFvf::grcFvfChannels)k;
								if (declaration->getChannelIsInUsed(channel)) {
									rage::grcFvf::grcDataSize newType = (rage::grcFvf::grcDataSize)declaration->getDynamicDataSizeType(channel);
									declaration->m_fvfChannelSizes.qwVal = setBits(declaration->m_fvfChannelSizes.qwVal, k * 4, newType, 4);
								}
								else 
									declaration->m_fvfChannelSizes.qwVal = setBits(declaration->m_fvfChannelSizes.qwVal, k * 4, 0, 4);
							}
							if (declaration->getChannelIsInUsed(rage::grcFvf::grcFvfChannels::grcfcTangent0) || 
								declaration->getChannelIsInUsed(rage::grcFvf::grcFvfChannels::grcfcTangent1)) {
								declaration->m_fvfChannelSizes.qwVal = 
									declaration->m_fvfChannelSizes.qwVal = 
									setBits(declaration->m_fvfChannelSizes.qwVal, 14 * 4, declaration->getDynamicDataSizeType(rage::grcFvf::grcFvfChannels::grcfcTangent0), 4);
							}
							else
								declaration->m_fvfChannelSizes.qwVal = setBits(declaration->m_fvfChannelSizes.qwVal, 14 * 4, 0, 4);

							if (declaration->getChannelIsInUsed(rage::grcFvf::grcFvfChannels::grcfcBinormal0) || 
								declaration->getChannelIsInUsed(rage::grcFvf::grcFvfChannels::grcfcBinormal0)) {
								declaration->m_fvfChannelSizes.qwVal = 
									declaration->m_fvfChannelSizes.qwVal = 
									setBits(declaration->m_fvfChannelSizes.qwVal, 15 * 4, declaration->getDynamicDataSizeType(rage::grcFvf::grcFvfChannels::grcfcBinormal0), 4);
							}
							else
								declaration->m_fvfChannelSizes.qwVal = setBits(declaration->m_fvfChannelSizes.qwVal, 15 * 4, 0, 4);

							declaration->m_nbFvfSize = declaration->m_nbChannelCount * 0x10; // aligned :-)

							DWORD dwNewMemSize = declaration->m_nbFvfSize * vertBuf->m_wVertexCount;
							BYTE* pNewMem = libertyFourXYZ::g_memory_manager.allocate<BYTE>("processwdr, new verticles, convert to dynamic", dwNewMemSize);
							BYTE* pOldMem = vertBuf->m_pVertexData.pElement;

							for (WORD l = 0; l < vertBuf->m_wVertexCount; l++) {
								DWORD dwPosInOldBuf = l * oldDecl.m_nbFvfSize;
								DWORD dwPosInNewBuf = l * declaration->m_nbFvfSize;
								for (BYTE k = 0; k < 18; k++) {
									if ((declaration->m_fvf.dwVal >> s_DynamicOrder[k]) & 1) {
										BYTE type = (oldDecl.m_fvfChannelSizes.qwVal >> (s_DynamicOrder[k] * 4)) & 0xf;
										Vector4 vec;
										
										if (type >= 0 && type <= 3) { // half
											rage::Half4* halfVec = (rage::Half4*)(pOldMem + dwPosInOldBuf + oldDecl.getOffset(s_DynamicOrder[k]));
											vec.x = halfVec->x;
											if(type >= 1) vec.y = halfVec->y;
											if (type >= 2) vec.z = halfVec->z;
											if (type >= 3) vec.w = halfVec->w;

											BYTE VecSize = rage::sm_TypeSizes[type];

										}
										else if (type >= 4 && type <= 7) { // float
											memcpy(&vec, pOldMem + dwPosInOldBuf + oldDecl.getOffset(s_DynamicOrder[k]), rage::sm_TypeSizes[type]);
										}
										else if (type == 8 || type == 9) { // color32 and ubyte. color32 is 32bit value!
											BYTE* pColor = (BYTE*)(pOldMem + dwPosInOldBuf + oldDecl.getOffset(s_DynamicOrder[k]));
											vec.x = pColor[0] * 255;
											vec.y = pColor[1] * 255;
											vec.z = pColor[2] * 255;
											vec.w = pColor[3] * 255;
										}


										type = (declaration->m_fvfChannelSizes.qwVal >> (s_DynamicOrder[k] * 4)) & 0xf;
										if (type >= 4 && type <= 7) { // float
											memcpy(pNewMem + dwPosInNewBuf, &vec, rage::sm_TypeSizes[type]);
										}
										else if (type == 8 || type == 9) { // color32 and ubyte. color32 is 32bit value!
											BYTE* pColor = (BYTE*)(pNewMem + dwPosInNewBuf);
											pColor[0] = vec.x * 255;
											pColor[1] = vec.y * 255;
											pColor[2] = vec.z * 255;
											pColor[3] = vec.w * 255;
										}
										dwPosInNewBuf += 0x10;
									}
								}
								dwPosInOldBuf += oldDecl.m_nbFvfSize;
							}

							declaration->m_bDynamicOrder = 1;
							geom->m_wStride = vertBuf->m_dwVertexSize = declaration->m_nbFvfSize;
							libertyFourXYZ::g_memory_manager.release(vertBuf->m_pVertexData.pElement);
							vertBuf->m_pVertexData.pElement = pNewMem;


						}
					}
				}
			}
		}

		// delete models and skel
		if (false) {
			for (BYTE i = 0; i < 4; i++) {
				if (pDrawable->m_lodgroup.m_lod[i].get())
					pDrawable->m_lodgroup.m_lod[i].destroy();
			}
			if (pDrawable->m_pSkeleton)
				libertyFourXYZ::g_memory_manager.release(pDrawable->m_pSkeleton);
		}

		// fix models with my custom terrain shaders
		if (true) {
			for (BYTE i = 0; i < 4; i++) {
				if (!pDrawable->m_lodgroup.m_lod.m_elements[i].pElement) continue;

				for (WORD j = 0; j < pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models.m_count; j++) {
					auto model = pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get();
					for (WORD n = 0; n < model->m_geometries.m_count; n++) {
						auto shader = pDrawable->m_pShaderGroup->m_shaders[model->m_pawShaderMappings[n]].get();
						if (shader->m_pszName != "gta_terrain_cb_w_4lyr_2tex_blend_spm" && shader->m_pszName != "gta_terrain_cb_w_4lyr_spm") continue; // skip non my shaders

						auto geom = pDrawable->m_lodgroup.m_lod.m_elements[i].get()->m_models[j].get()->m_geometries[n].get();


						for (BYTE o = 0; o < 4; o++) {
							if (!geom->m_vertexBuffers[o].get()) continue;

							auto vertBuf = geom->m_vertexBuffers[o].get();

							auto declaration = vertBuf->m_pDeclarations.get();

							if (!declaration->m_fvf.m_bTexCoord4 || !declaration->m_fvf.m_bTexCoord5) continue;
							if (declaration->m_fvf.m_bSpecular)
								trace("[processWdr] specular channel is in use, it will be overwritten");

							rage::grcFvf oldDecl;
							memcpy(&oldDecl, declaration, sizeof * declaration);

							declaration->m_fvf.m_bSpecular = 1;

							declaration->m_fvf.m_bTexCoord2 = 0;
							declaration->m_fvf.m_bTexCoord3 = 0;
							declaration->m_fvf.m_bTexCoord4 = 0;
							declaration->m_fvf.m_bTexCoord5 = 0;
							declaration->m_fvf.m_bTexCoord6 = 0;
							declaration->m_fvf.m_bTexCoord7 = 0;

							declaration->recomputeTotalSize();

							if(pDrawable->m_pShaderGroup->m_vertexFormat.m_count)
								pDrawable->m_pShaderGroup->m_vertexFormat[model->m_pawShaderMappings[n]] = declaration->m_fvf.dwVal;

							DWORD dwNewMemSize = declaration->m_nbFvfSize * vertBuf->m_wVertexCount;
							BYTE* pNewMem = libertyFourXYZ::g_memory_manager.allocate<BYTE>("processwdr, fix my shaders", dwNewMemSize);
							BYTE* pOldMem = vertBuf->m_pVertexData.pElement;

							for (WORD l = 0; l < vertBuf->m_wVertexCount; l++) {
								DWORD dwPosInOldBuf = l * oldDecl.m_nbFvfSize;
								DWORD dwPosInNewBuf = l * declaration->m_nbFvfSize;
								for (BYTE k = 0; k < 18; k++) {
									auto channel = (grcFvf::grcFvfChannels)k;
									if ((declaration->m_fvf.dwVal >> k) & 1) {
										if (channel != grcFvf::grcFvfChannels::grcfcSpecular) {
											DWORD dwElementSize = oldDecl.getSize(channel);
											memcpy(pNewMem + dwPosInNewBuf + declaration->getOffset(channel),
												pOldMem + dwPosInOldBuf + oldDecl.getOffset(channel), dwElementSize);
										}
										else {
											//BYTE type = (oldDecl.m_fvfChannelSizes.qwVal >> (k * 4)) & 0xf;
											BYTE uv4type = (oldDecl.m_fvfChannelSizes.qwVal >> (grcFvf::grcFvfChannels::grcfcTexture4 * 4)) & 0xf;
											BYTE uv5type = (oldDecl.m_fvfChannelSizes.qwVal >> (grcFvf::grcFvfChannels::grcfcTexture5 * 4)) & 0xf;
											Vector4 vec;
											if (uv4type >= 0 && uv4type <= 3) { // half
												rage::Half4* halfVec = (rage::Half4*)(pOldMem + dwPosInOldBuf + oldDecl.getOffset(grcFvf::grcFvfChannels::grcfcTexture4));
												vec.x = halfVec->x;
												if (uv4type >= 1) vec.y = halfVec->y;
											}
											else if (uv4type >= 4 && uv4type <= 7) { // float
												memcpy(&vec, pOldMem + dwPosInOldBuf + oldDecl.getOffset(grcFvf::grcFvfChannels::grcfcTexture4),
													rage::sm_TypeSizes[uv4type <= 5 ? uv4type : 5]);
											}

											if (uv5type >= 0 && uv5type <= 3) { // half
												rage::Half4* halfVec = (rage::Half4*)(pOldMem + dwPosInOldBuf + oldDecl.getOffset(grcFvf::grcFvfChannels::grcfcTexture5));
												vec.z = halfVec->z;
												if (uv5type >= 1) vec.z = halfVec->z;
											}
											else if (uv5type >= 4 && uv5type <= 7) { // float
												memcpy(&vec.z, pOldMem + dwPosInOldBuf + oldDecl.getOffset(grcFvf::grcFvfChannels::grcfcTexture5),
													rage::sm_TypeSizes[uv5type <= 5 ? uv5type : 5]);
											}


											//else if (type == 8 || type == 9) { // color32 and ubyte. color32 is 32bit value!
											//	BYTE* pColor = (BYTE*)(pOldMem + dwPosInOldBuf + oldDecl.getOffset(channel));
											//	vec.x = pColor[0] * 255;
											//	vec.y = pColor[1] * 255;
											//	vec.z = pColor[2] * 255;
											//	vec.w = pColor[3] * 255;
											//}

											bool useDifOrder = 1;
											bool bFixRedAndBlue = 1;
											
											BYTE* pSpecular = (BYTE*)(pNewMem + dwPosInNewBuf + declaration->getOffset(channel));
											if (!useDifOrder) {
												pSpecular[0] = vec.x * 255;
												pSpecular[1] = vec.y * 255;
												pSpecular[2] = vec.z * 255;
												pSpecular[3] = vec.w * 255;
											}
											else {
												pSpecular[3] = vec.x * 255;
												pSpecular[0] = vec.y * 255;
												pSpecular[1] = vec.z * 255;
												pSpecular[2] = vec.w * 255;
											}
											if (bFixRedAndBlue) {
												BYTE tmp = pSpecular[0];
												pSpecular[0] = pSpecular[2];
												pSpecular[2] = tmp;
											}

											//memset(pSpecular, 0x0, 4);
											//pSpecular[2] = 0xff;
											//for (BYTE m = 0; m < 4; m++) {
											//	if (l % 4 == m)
											//		pSpecular[m] = 0xff;
											//	else pSpecular[m] = 0x0;
											//}

										}
									}
								}
							}
							geom->m_wStride = vertBuf->m_dwVertexSize = declaration->m_nbFvfSize;
							libertyFourXYZ::g_memory_manager.release(vertBuf->m_pVertexData.pElement);
							vertBuf->m_pVertexData.pElement = pNewMem;

						}
					}
				}
			}
		}

		if (true) {
			if (pDrawable->m_pShaderGroup) {
				pDrawable->m_pShaderGroup->m_vertexFormat.destroy();
				pDrawable->m_pShaderGroup->m_indexMapping.destroy();
			}
		}

		pDrawable->setRefCount();
		
		pLayout->setMainObject(pDrawable);
		pDrawable->addToLayout(pLayout, 0);
		pLayout->create();

		pResource = libertyFourXYZ::g_memory_manager.allocate<rage::datResource>("processWdr, pResource2");
		pResource->m_pszDebugName = pszResName;
		pResource->m_pMap->validateMap(pLayout->getResourceInfo());

		pDrawable->replacePtrs(pLayout, pResource, 0);
		memcpy(pResource->getFixup((gtaDrawable*)pLayout->mainObj.second, sizeof(*pDrawable)), pDrawable, sizeof(*pDrawable));
		pLayout->setOldPtrs();


		pResource->saveRawResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), 1);
		pResource->saveResource(pszOutFile.getFilePath(), pszOutFile.getFileNameWithoutExt(), "wdr", 110, pLayout->getResourceInfo(), 0);

		pResource->m_pMap->printMap(trace);
		libertyFourXYZ::g_memory_manager.release(pResource);

		libertyFourXYZ::g_memory_manager.release(pLayout);

		libertyFourXYZ::g_memory_manager.release(pDrawable);



	}

}