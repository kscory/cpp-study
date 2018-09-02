#include "Player.h"
#include "MapManager.h"
#include "Stage.h"
#include "ObjectManager.h"

CPlayer::CPlayer() {

}

CPlayer::~CPlayer() {

}

bool CPlayer::Init() {

	m_tPos.x = 4;
	m_tPos.y = 8;

	m_bJump = false;
	m_bComplete = false;
	m_iJumpDir = JD_STOP;
	m_iJumpState = 0;
	m_iScore = 0;
	m_bBulletFire = false;
	m_bBigItem = false;

	return true;
}

void CPlayer::Update() {

	CStage* pStage = CMapManager::GetInst()->GetStage();

	// Ű �Է��� �޴´�.
	// GetAsyncKeyState �Լ��� win32 API ���� �����Ǵ� Ű �Է� �Լ�(MAC ��... �̤� �ϴ� ��)
	// A Ű�� ������ �� �ٷ� �����ϰ� �ϱ� ���ؼ� 0x8000 �� & ������ �Ͽ� ������ ��� true �� ������ �ȴ�.
	if (GetAsyncKeyState('A') & 0x8000) {

		if (pStage->GetBlock(m_tPos.x - 1, m_tPos.y) != SBT_WALL) {

			if (!m_bBigItem) {
				--m_tPos.x;
			}
			else {
				// Ŀ���� �������� ȹ���� ���¶�� �ٷ� ��ĭ�� ���ʵ� üũ
				if (pStage->GetBlock(m_tPos.x - 1, m_tPos.y - 1) != SBT_WALL) {
					--m_tPos.x;
				}
			}

			if (m_tPos.x < 0)
				m_tPos.x = 0;
		}
	}

	if (GetAsyncKeyState('D') & 0x8000) {
		if (pStage->GetBlock(m_tPos.x + 1, m_tPos.y) != SBT_WALL) {

			if (!m_bBigItem) {
				++m_tPos.x;
			}
			else {
				// Ŀ���� �������� ȹ���� ���¶�� �ٷ� ��ĭ�� �����ʵ� üũ
				if (pStage->GetBlock(m_tPos.x + 1, m_tPos.y - 1) != SBT_WALL) {
					++m_tPos.x;
				}
			}

			if (m_tPos.x >= BLOCK_X)
				m_tPos.x = 49;
		}
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !m_bJump) {
		m_bJump = true;
		m_iJumpDir = JD_UP;
		m_iJumpState = 0;
	}

	int iBigCount = 1;
	// Ŀ�������� üũ�ϱ� ���� �ڵ�
	if (m_bBigItem) {
		iBigCount = 2;
	}

	// jump �� true ���?
	if (m_bJump) {
		switch (m_iJumpDir) {
		case JD_UP:
			m_iJumpState++;

			if (m_iJumpState > JUMP_BLOCK_MAX) {
				m_iJumpState = JUMP_BLOCK_MAX;
				m_iJumpDir = JD_DOWN;
			}
			else if (pStage->GetBlock(m_tPos.x, m_tPos.y - iBigCount) == SBT_WALL) {
				m_iJumpState--;
				// ���� ���� �� �������� �����ϰ� �� �� �ϳ��� ������ �Ѵ�.
				int iRand = rand() % 100;
				STAGE_BLOCK_TYPE eBlockType;

				if (iRand < 70) {
					eBlockType = SBT_ITEM_BULLET;
				} else {
					eBlockType = SBT_ITEM_BIG;
				}

				pStage->ChangeBlock(m_tPos.x, m_tPos.y - iBigCount, eBlockType);
				m_iJumpDir = JD_DOWN;
			}
			else {
				m_tPos.y--;
			}
			break;
		case JD_DOWN:
			if (m_tPos.y >= BLOCK_Y) {
				cout << "�÷��̾� ���" << endl;
				system("pause");
				m_tPos.y = BLOCK_Y - 1;
			} else if (pStage->GetBlock(m_tPos.x, m_tPos.y + 1) == SBT_WALL) {
				m_iJumpDir = JD_STOP;
				m_bJump = false;
			}
			else {
				m_tPos.y++;
			}
			break;
		}
	}

	// �������°� �ƴϰ� �Ʒ��� ���� �ƴϸ� ����߸���.
	if (pStage->GetBlock(m_tPos.x, m_tPos.y + 1) != SBT_WALL && !m_bJump) {
		m_tPos.y++;
	}

	STAGE_BLOCK_TYPE eCurBlockType = (STAGE_BLOCK_TYPE)pStage->GetBlock(m_tPos.x, m_tPos.y);
	STAGE_BLOCK_TYPE eUpBlockType = (STAGE_BLOCK_TYPE)pStage->GetBlock(m_tPos.x, m_tPos.y -1);

	// ���� ���� ���
	if (eCurBlockType == SBT_COIN || 
		(eUpBlockType == SBT_COIN && m_bBigItem)) {
		pStage->ChangeBlock(m_tPos.x, m_tPos.y, SBT_ROAD);
		pStage->ChangeBlock(m_tPos.x, m_tPos.y-1, SBT_ROAD);
		m_iScore += 1000;
	}
	// �Ѿ� ���� ���
	else if (eCurBlockType == SBT_ITEM_BULLET ||
		(eUpBlockType == SBT_ITEM_BULLET && m_bBigItem)) {
		m_bBulletFire = true;
		pStage->ChangeBlock(m_tPos.x, m_tPos.y, SBT_ROAD);
		pStage->ChangeBlock(m_tPos.x, m_tPos.y-1, SBT_ROAD);
	}
	// Ŀ���� ������ ���� ���
	else if (eCurBlockType == SBT_ITEM_BIG ||
		(eUpBlockType == SBT_ITEM_BIG && m_bBigItem)) {
		m_bBigItem = true;
		pStage->ChangeBlock(m_tPos.x, m_tPos.y, SBT_ROAD);
		pStage->ChangeBlock(m_tPos.x, m_tPos.y - 1, SBT_ROAD);
	}
	// ���� ������ ������ ���
	else if (eCurBlockType == SBT_END ||
		(eUpBlockType == SBT_END && m_bBigItem)) {
		m_bComplete = true;
	}

	if (m_tPos.y >= BLOCK_Y) {
		cout << "�÷��̾� ���" << endl;
		m_tPos = pStage->GetStart();
		Reset();
		pStage->ResetStage();
		system("pause");
		return;
	}

	// ���콺 ������ ������ �Ѿ� �߻�
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && m_bBulletFire) {
		// �� ��ġ���� ��ĭ �տ� �Ѿ��� �ִ´�.
		POINT tPos = m_tPos;
		tPos.x++;
		CObjectManager::GetInst()->CreateBullet(tPos);


	}
}