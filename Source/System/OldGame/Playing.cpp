#include "Playing.h"
#include "GameSystem.h"

GameSystem* gameptr;

_Playing::_Playing(GameSystem* ptr) {
	::gameptr = ptr;
}

_Playing::~_Playing() {

}

void GameSystem::PlayingInit() {

	SetState(
		"Playing:" + 
		Playing.Chart.OriginalData.Title + 
		"[" + 
		SongSelect.CourseList[SongSelect.CourseIndex] + 
		"]"
	);

	for (auto&& taiko : Playing.MiniTaikoFlash) {
		taiko.End();
	}
}

void GameSystem::PlayingEnd() {

	Playing.Chart.SongData.Delete();

}

void GameSystem::PlayingDraw() {

	const double NowTime = Playing.ChartNowTime(Config.FrameCountTimer, Config.FastDrawRate, Config.FrameExtendRate) + Playing.TrainingOffset;

	Skin.Base->Playing.Image.BackGround.Draw({ 0,0 });

	int pldx = 0;

	do {

		PlayerData MultiData;
		if (Private.CountAll > 1) {
			MultiData = MultiDatas[pldx];
		}

		Pos2D<float> add = { 0.0f, 180.0f * (float)pldx};
		add.Y -= 155.0f * (Private.CountAll == 4);

		auto& HitNote = Playing.HitNote[pldx];

		Skin.Base->Playing.Image.LaneFrame.Draw(add, 0);
		Skin.Base->Playing.Image.Lane.Draw(add, 0);

		Playing.BranchLaneDraw(pldx, add);

		if (!SongSelect.IsDanMode) {
			Playing.ProgressBarDraw(pldx, add);
		}
		Skin.Base->Playing.Image.Note.Draw(add, 0);

		if (Playing.Chart.NowGoGo) {

			uint drawindex = NowTime / Skin.Base->Playing.Config.GoGoFireFrameTime;

			Skin.Base->Playing.Image.GoGoFire.Draw(
				{
				Skin.Base->Playing.Image.GoGoFire.Pos.X,
				Skin.Base->Playing.Image.GoGoFire.Pos.Y + add.Y
				},
				drawindex % Skin.Base->Playing.Image.GoGoFire.Div.X);

		}

		const Pos2D<double>& NoteOrigin = {
			Skin.Base->Playing.Image.Note.Pos.X,
			Skin.Base->Playing.Image.Note.Pos.Y + add.Y
		};

		auto GetNotePos = [&](NoteData& data)->Pos2D<double> {
			bool BMScroll = ScrollType::BMSCROLL == Playing.Chart.ScrollType;
			Pos2D<double> _ret;
			if (ScrollType::Normal == Playing.Chart.ScrollType) {
				double _temp = ((data.AbsTime - NowTime) / (240 / data.BPM));
				_ret = { _temp,_temp };
			}
			else {
				double _bpm = data.BMFlag || data.BpmSpawnFlag ? data.BPM : Playing.Chart.NowBPM;
				double optime = (((data.BMFlag || data.BpmSpawnFlag ? data.AbsTime : data.BMTime) - NowTime) / (240 / _bpm));
				_ret = { optime,optime };
			}

			_ret = {
				_ret.X *= Skin.Base->Playing.Config.LaneExtendRate * (BMScroll ? 1 : data.Scroll),
				_ret.Y *= Skin.Base->Playing.Config.LaneExtendRate * (BMScroll ? 0 : data.Scrolli)
			};

			Pos2D<float> SkinPos = {
				Skin.Base->Playing.Image.Lane.Size.Width,
				Skin.Base->Playing.Image.Lane.Size.Height
			};

			BranchLevel level = Playing.Chart.Judge[pldx].Level > BranchLevel::None ? Playing.Chart.Judge[pldx].Level : BranchLevel::Normal;
			int Dir = Playing.Chart.Judge[pldx].Level < Playing.Chart.Judge[pldx].PrevLevel ? 1 : -1;
			double Offset = 0.0f;
			if (Skin.Base->Playing.Config.BranchSlideAnimation) {
				if (Playing.BranchLaneMove.GetNowRecording() && data.BranchLevel != BranchLevel::None) {
					Offset = SkinPos.Y * -(float)data.BranchLevel + SkinPos.Y * (((int)level + Dir) - GetEasingRate(Playing.BranchLaneMove.GetRecordingTime() / Skin.Base->Playing.Config.BranchSlideTime, ease::Base::In, ease::Line::Sine) * Dir);
				}
			}
			else {
				if (data.BranchLevel != BranchLevel::None) {
					Offset = SkinPos.Y * -(float)data.BranchLevel + SkinPos.Y * (float)Playing.Chart.Judge[pldx].Level;
				}
			}

			_ret = {
				_ret.X + NoteOrigin.X,
				_ret.Y + NoteOrigin.Y + Offset
			};

			return _ret;

			};

		{
			int i = HitNote.Index;
			const double JudgeUnderExplosionTime = Skin.Base->Playing.Config.JudgeUpperExplosionFrameTime * Skin.Base->Playing.Image.JudgeUnderExplosion.Div.X;

			for (int c = 0; c < HitNote.Size(); ++c) {
				auto&& data = HitNote.Datas[i];
				if (!data.MoveTimer.GetNowRecording()) {
					data.MoveTimer.Start();
				}

				data.MoveElapsedTime = data.MoveTimer.GetRecordingTime() / TimerType::microsecond;

				if (data.JudgeUnderExplosion.IsActive && data.MoveElapsedTime < JudgeUnderExplosionTime) {

					uint drawindex = data.MoveElapsedTime / Skin.Base->Playing.Config.JudgeUpperExplosionFrameTime;

					drawindex += (2 * Skin.Base->Playing.Image.JudgeUnderExplosion.Div.X) * data.JudgeUnderExplosion.Big;

					if (data.JudgeUnderExplosion.Type == JudgeType::Ok) {
						drawindex += Skin.Base->Playing.Image.JudgeUnderExplosion.Div.X;
					}

					Skin.Base->Playing.Image.JudgeUnderExplosion.Draw(add, drawindex);

				}
				else {
					data.JudgeUnderExplosion.IsActive = false;
				}

				++i;
				if (!(i < HitNote.Size())) {
					i = 0;
				}
			}
		}

		{
			auto& ProcNotes = !pldx ? Playing.Chart.RawNoteDatas : MultiData.RawNoteDatas;
			double _addms = ProcNotes[0].AbsTime;
			for (int i = 0, size = ProcNotes.size(); i < size; ++i) {
				NoteData& data = ProcNotes[i];

				if (data.AbsTime < NowTime) {
					data.BMFlag = true;
					Playing.Chart.NowBPM = data.BPM;
				}

				if (data.BpmChangeFlag) {
					if (data.BPM * data.Measure > 0) {
						for (int j = i + 1; j < size; ++j) {
							auto& jdata = ProcNotes[j];
							if (jdata.BpmChangeFlag && jdata.BPM * jdata.Measure < 0) {
								data.BpmChangeFlag = false;
								for (int k = j; k < size; ++k) {
									auto& kdata = ProcNotes[k];
									if (kdata.AbsTime < jdata.AbsTime) {
										kdata.BpmSpawnFlag = true;
									}
									else {
										kdata.BpmSpawnFlag = false;
									}
								}
								break;
							}
							if (jdata.BpmChangeFlag && jdata.BPM * jdata.Measure > 0) {
								data.BpmChangeFlag = false;
								for (int k = j; k < size; ++k) {
									ProcNotes[k].BpmSpawnFlag = false;
								}
								break;
							}
						}
					}
					else {
						data.BpmChangeFlag = false;
					}
				}

				if (data.BMFlag || i == 0) { _addms = data.AbsTime; data.BMTime = data.AbsTime; continue; }

				double _bpm = (Playing.Chart.NowBPM / ProcNotes[i - 1].BPM);
				_addms += ProcNotes[i - 1].RelaTime / _bpm;
				data.BMTime = _addms;
			}
		}

#define InRange(x, y) (x > Skin.SimulationRect.Left && x < Skin.SimulationRect.Right && y > Skin.SimulationRect.Top && y < Skin.SimulationRect.Bottom)

		std::complex<double> n1{};
		std::complex<double> n2{};
		std::complex<double> n3{};
		std::complex<double> n4{};
		std::complex<double> facing{};
		Pos2D<double> NotePos{};

		auto& NoteDatas = !pldx ? Playing.Chart.RawNoteDatas : MultiData.RawNoteDatas;
		const std::complex<double> n0 = { Skin.Base->Playing.Image.Note.Size.Width / 2, Skin.Base->Playing.Image.Note.Size.Height / 2 };
		const double n0r = std::abs(n0);
		const double narr[4] = {
			std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * -1 }),
			std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * -1 }),
			std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * 1 }),
			std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * 1 })
		};

		Playing.BranchLaneClip(Playing.Chart.IsBranchedChart, add);
		for (auto&& data : NoteDatas | std::ranges::views::reverse) {

			double NoteTheta = atan2(data.Scrolli, data.Scroll);

			NotePos = GetNotePos(data);

			if (data.BarlineDisplay) {

				if (InRange(NotePos.X, NotePos.Y)) {
					DrawLineAA(
						NotePos.X,
						NotePos.Y - 65,
						NotePos.X,
						NotePos.Y + 65,
						GetColor(255, 255, 255 * !data.BranchStart)
					);
				}
			}

			if (data.NoteType == '0') {
				continue;
			}

			if (data.NoteType >= '1' &&
				data.NoteType <= '4') {
				if (InRange(NotePos.X, NotePos.Y)) {
					int NoteAlpha = 255;
					double hidden = Config.HiddenLevel > 0 ? Config.HiddenLevel : MultiRoom.MultiFlag ? MultiData.Option.Hidden : 0;
					double sudden = Config.SuddenLevel > 0 ? Config.SuddenLevel : MultiRoom.MultiFlag ? MultiData.Option.Sudden : 0;
					bool hiddenflag = hidden > 0;
					bool suddenflag = sudden > 0;
					bool multiflag = pldx;
					if (hiddenflag || suddenflag || multiflag) {
						double _abs = std::abs(std::complex<double>{ NotePos.X - NoteOrigin.X, NotePos.Y - NoteOrigin.Y })* (data.AbsTime < NowTime ? -1 : 1);
						double leveling = (Skin.Base->Playing.Image.Lane.Size.Width / DX_PI);
						double feedrange = (Skin.Base->Playing.Image.Lane.Size.Width / DX_TWO_PI);
						if (multiflag) {
							Playing.NoteAlpha(NoteAlpha, (_abs - (leveling * 0.025)) / feedrange, AlphaType::Hidden);
						}
						if (hiddenflag) {
							Playing.NoteAlpha(NoteAlpha, (_abs - (leveling * hidden)) / feedrange, AlphaType::Hidden);
						}
						if (suddenflag) {
							Playing.NoteAlpha(NoteAlpha, ((_abs - (Skin.Base->Playing.Image.Lane.Size.Width)) + (leveling * sudden)) / feedrange, AlphaType::Sudden);
						}
					}

					Skin.Base->Playing.Image.Note.Draw(
						{
							(float)(NotePos.X - NoteOrigin.X),
							(float)(NotePos.Y - NoteOrigin.Y) + add.Y
						},
						data.NoteType - 48
					);
				}
				continue;
			}
			SetDrawBlendMode(0, 0);

			if (data.NoteType >= '5' &&
				data.NoteType <= '6') {
				const Pos2D<double>& cnote = NotePos;
				const Pos2D<double>& dnote = GetNotePos(NoteDatas[data.RollEndIndex]);

				bool DispFlag =
					InRange(cnote.X, cnote.Y) ||
					(data.AbsTime < NowTime && data.RollEndTime > NowTime) ||
					InRange(dnote.X, dnote.Y);

				if (DispFlag) {

					bool BigRollFlag = data.NoteType == '6';

					const std::complex<double>& cdnote = { dnote.X - cnote.X, dnote.Y - cnote.Y };

					double RollTheta = std::arg(cdnote);
					facing = std::polar(n0.real() - 3, RollTheta);

					n1 = std::polar(n0r, narr[0] + RollTheta);
					n2 = std::polar(n0r, narr[1] + RollTheta);
					n3 = std::polar(n0r, narr[2] + RollTheta);
					n4 = std::polar(n0r, narr[3] + RollTheta);

					DrawModiGraphF(
						cnote.X + facing.real() + n1.real(),
						cnote.Y + facing.imag() + n1.imag(),
						dnote.X - facing.real() + n2.real(),
						dnote.Y - facing.imag() + n2.imag(),
						dnote.X - facing.real() + n3.real(),
						dnote.Y - facing.imag() + n3.imag(),
						cnote.X + facing.real() + n4.real(),
						cnote.Y + facing.imag() + n4.imag(),
						Skin.Base->Playing.Image.Note.Handles[BigRollFlag ? 9 : 6],
						TRUE
					);
					DrawModiGraphF(
						dnote.X + n1.real(),
						dnote.Y + n1.imag(),
						dnote.X + n2.real(),
						dnote.Y + n2.imag(),
						dnote.X + n3.real(),
						dnote.Y + n3.imag(),
						dnote.X + n4.real(),
						dnote.Y + n4.imag(),
						Skin.Base->Playing.Image.Note.Handles[BigRollFlag ? 10 : 7],
						TRUE
					);
					DrawModiGraphF(
						cnote.X + n1.real(),
						cnote.Y + n1.imag(),
						cnote.X + n2.real(),
						cnote.Y + n2.imag(),
						cnote.X + n3.real(),
						cnote.Y + n3.imag(),
						cnote.X + n4.real(),
						cnote.Y + n4.imag(),
						Skin.Base->Playing.Image.Note.Handles[BigRollFlag ? 8 : 5],
						TRUE
					);

				}
			}

			if (data.NoteType == '7' ||
				data.NoteType == '9') {

				if (data.BalloonFlag == 1) {
					NotePos = NoteOrigin;
				}
				if (data.BalloonFlag == 2) {
					NotePos = GetNotePos(NoteDatas[data.RollEndIndex]);
				}

				if (InRange(NotePos.X, NotePos.Y)) {

					bool KusudamaFlag = data.NoteType == '9';

					facing = std::polar(n0.real() * 2, NoteTheta);

					n1 = std::polar(n0r, narr[0] + NoteTheta);
					n2 = std::polar(n0r, narr[1] + NoteTheta);
					n3 = std::polar(n0r, narr[2] + NoteTheta);
					n4 = std::polar(n0r, narr[3] + NoteTheta);

					DrawModiGraphF(
						NotePos.X + n1.real(),
						NotePos.Y + n1.imag(),
						NotePos.X + n2.real(),
						NotePos.Y + n2.imag(),
						NotePos.X + n3.real(),
						NotePos.Y + n3.imag(),
						NotePos.X + n4.real(),
						NotePos.Y + n4.imag(),
						Skin.Base->Playing.Image.Note.Handles[KusudamaFlag ? 13 : 11],
						TRUE
					);
					DrawModiGraphF(
						NotePos.X + facing.real() + n1.real(),
						NotePos.Y + facing.imag() + n1.imag(),
						NotePos.X + facing.real() + n2.real(),
						NotePos.Y + facing.imag() + n2.imag(),
						NotePos.X + facing.real() + n3.real(),
						NotePos.Y + facing.imag() + n3.imag(),
						NotePos.X + facing.real() + n4.real(),
						NotePos.Y + facing.imag() + n4.imag(),
						Skin.Base->Playing.Image.Note.Handles[KusudamaFlag ? 14 : 12],
						TRUE
					);
				}
			}
		}
		Playing.BranchLaneClip(false, add);

		Skin.Base->Playing.Image.Base.Draw(add, 0);
		Skin.Base->Playing.Image.NamePlate.Draw(add, 0);
		Skin.Base->Playing.Image.MiniTaiko.Draw(add, 0);

		if (!MultiRoom.MultiFlag) { Playing.NameDraw(Config.PlayerName, add); }
		else { Playing.NameDraw(MultiData.Name, add); }

		{
			int i = HitNote.Index;

			for (int c = 0; c < HitNote.Size(); ++c) {
				auto& data = HitNote.Datas[i];
				if (!data.MoveTimer.GetNowRecording()) {
					data.MoveTimer.Start();
				}

				if (data.FlyingNote.IsActive && Config.HitNoteDisp && data.MoveElapsedTime < data.FlyingNote.MoveTime()) {

					float _one = (data.MoveElapsedTime / data.FlyingNote.MoveTime());

					std::complex<float> _pos1 = { 840, -90 };
					std::complex<float> _pos2 = std::polar(280.0f, (DX_PI_F / 2) + std::arg(_pos1 - add.Y));
					std::complex<float> _r = { (_pos1.real() / 2) + _pos2.real(), (_pos1.imag() / 2) + _pos2.imag() };
					float mem0arg = std::arg(_r);
					_r += { -840, 90 };
					float mem1arg = std::arg(_r);
					_r = { (_pos1.real() / 2) + _pos2.real(), (_pos1.imag() / 2) + _pos2.imag() };
					float allarg = mem1arg - mem0arg;
					std::complex<float> _c1 = std::polar(std::abs(_r), allarg * _one + mem0arg);

					float imagpos = !pldx ? _r.imag() - _c1.imag() : _c1.imag() - _r.imag();

					Pos2D<float> Pos = {
							_r.real() - _c1.real(),
							imagpos + add.Y
					};

					if (Private.CountAll < 3) {
						Skin.Base->Playing.Image.Note.Draw(Pos, data.FlyingNote.Type - 48);
					}
				}
				else {
					data.FlyingNote.IsActive = false;
				}

				if (data.JudgeString.IsActive && data.MoveElapsedTime < data.JudgeString.MoveTime()) {
					double alpha = 255 * (1 - GetEasingRate(data.MoveElapsedTime / data.JudgeString.MoveTime(), ease::Base::In, ease::Line::Cubic));
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
					Skin.Base->Playing.Image.JudgeString.Draw(add, (int)data.JudgeString.Type);
					SetDrawBlendMode(0, 0);
				}
				else {
					data.JudgeString.IsActive = false;
				}

				if (!data.FlyingNote.IsActive && !data.JudgeUnderExplosion.IsActive && !data.JudgeString.IsActive) {
					data = HitNoteData();
				}

				++i;
				if (!(i < HitNote.Size())) {
					i = 0;
				}
			}
		}

#define TAIKOFLASH(type, dir, x) { type.Draw({type.Size.Width * dir, add.Y}, x); }

		if (Playing.MiniTaikoFlash[0 + 4 * pldx].GetNowRecording()) {
			Playing.TaikoAlpha(0 + 4 * pldx);
			TAIKOFLASH(Skin.Base->Playing.Image.MiniTaiko_Don, -0.5f, 0)
		}
		if (Playing.MiniTaikoFlash[1 + 4 * pldx].GetNowRecording()) {
			Playing.TaikoAlpha(1 + 4 * pldx);
			TAIKOFLASH(Skin.Base->Playing.Image.MiniTaiko_Ka, -0.5f, 0)
		}
		if (Playing.MiniTaikoFlash[2 + 4 * pldx].GetNowRecording()) {
			Playing.TaikoAlpha(2 + 4 * pldx);
			TAIKOFLASH(Skin.Base->Playing.Image.MiniTaiko_Don, 0.5f, 1)
		}
		if (Playing.MiniTaikoFlash[3 + 4 * pldx].GetNowRecording()) {
			Playing.TaikoAlpha(3 + 4 * pldx);
			TAIKOFLASH(Skin.Base->Playing.Image.MiniTaiko_Ka, 0.5f, 1)
		}
		SetDrawBlendMode(0, 0);

		if (Playing.Chart.BalloonCount > 0) {
			Playing.BalloonDraw(Playing.Chart.BalloonCount, add);
		}
		if (Playing.Chart.Judge[pldx].Combo >= 3) {
			Playing.ComboDraw(pldx, add);
		}

		Playing.ScoreDraw(pldx, add);

		pldx++;

	} while (pldx < Private.CountAll);

	if (Private.CountAll < 4) {
		if (!SongSelect.IsDanMode) {
			Playing.TitleDraw(Playing.Chart.OriginalData.Title, Playing.Chart.OriginalData.PlayingTitleStrlen);
			Playing.SubTitleDraw(Playing.Chart.OriginalData.SubTitle, Playing.Chart.OriginalData.PlayingSubTitleStrlen);
		}
		else {
			Playing.TitleDraw(Playing.Chart.OriginalData.DanTitle, Playing.Chart.OriginalData.DanTitleStrlen);
			Playing.SubTitleDraw(Playing.Chart.OriginalData.DanSubTitle, Playing.Chart.OriginalData.DanSubTitleStrlen);
		}
	}

	if (SongSelect.IsDanMode) {
		Skin.Base->Playing.Image.Box.Draw({ 0,0 });
		Playing.ExamProgressBarDraw();
		Playing.ExamValDraw();
	}

	if (Config.ViewDebugData) {
		DrawFormatString(0, 0, GetColor(255, 255, 255), "\n\n\nNowTime:%lf\nBPM:%lf", NowTime / Playing.Chart.SongSpeed, Playing.Chart.NowBPM * Playing.Chart.SongSpeed);
	}
}

void GameSystem::PlayingProc() {

	const double NowTime = Playing.ChartNowTime() + Playing.TrainingOffset;
	auto&& NoteDatas = Playing.Chart.RawNoteDatas;

	if (!MultiRoom.MultiFlag) {

		Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
			if (!SongSelect.IsDanMode) {
				Playing.TrainingOffset = 0;
				Playing.MeasureIndex = 0;
				NowScene = Scene::SongSelect;
			}
			else {
				Playing.Chart.IsDanPlay = false;
				NowScene = Scene::DanSelect;
			}
			});

		if (!SongSelect.IsDanMode) {

			Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
				Input.HitKeyProcess('Q', KeyState::Down, [&] {
					NowScene = Scene::Loading;
					});
				});

			if (!Config.TrainingMode) {

				Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
					Input.HitKeyProcess(VK_TAB, KeyState::Down, [&] {
						NowScene = Scene::Result;
						});
					});
			}
			else {

				if (!Playing.Chart.NowTime.GetNowRecording()) {

					if (!Playing.MeasureJump.GetNowRecording()) {

						static auto MoveInputProc = [&](bool direction) {
							while (!ProcessMessage()) {
								if (direction) {
									Playing.MeasureIndex < Playing.Chart.RawNoteDatas.size() - 1 ? Playing.MeasureIndex++ : Playing.MeasureIndex;
									if (Playing.Chart.RawNoteDatas[Playing.MeasureIndex].BarlineDisplay || Playing.MeasureIndex == Playing.Chart.RawNoteDatas.size() - 1) { break; }
								}
								else {
									Playing.MeasureIndex > 0 ? Playing.MeasureIndex-- : Playing.MeasureIndex;
									if (Playing.Chart.RawNoteDatas[Playing.MeasureIndex].BarlineDisplay || Playing.MeasureIndex == 0) { break; }
								}
							}
							Playing.MemNowTime = NowTime;
							Playing.MeasureJump.Start();
							};

						static auto BranchChangeProc = [&](bool direction) {
							Playing.Chart.Judge[0].PrevLevel = Playing.Chart.Judge[0].Level;
							if (!direction) {
								if (Playing.Chart.Judge[0].Level < BranchLevel::Master) {
									bool IsNone = Playing.Chart.Judge[0].Level == BranchLevel::None;
									Playing.Chart.Judge[0].Level = (BranchLevel)((int)Playing.Chart.Judge[0].Level + 1 + IsNone);
									Playing.BranchLaneMove.Start();
								}
							}
							else {
								if (Playing.Chart.Judge[0].Level > BranchLevel::Normal) {
									Playing.Chart.Judge[0].Level = (BranchLevel)((int)Playing.Chart.Judge[0].Level - 1);
									Playing.BranchLaneMove.Start();
								}
							}
						};

						static auto StartInputProc = [&] {
							Playing.Chart.NowTime.Start();
							Playing.Chart.FrameNowTime.Start();
							if (Playing.Chart.SongBlankTime < NowTime) {
								Playing.Chart.SongData.SetCurrent(NowTime - Playing.Chart.SongBlankTime);
								Playing.Chart.SongData.Play(FALSE);
							}
							};

						Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] { MoveInputProc(false); });
						Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] { MoveInputProc(true); });
						Input.HitKeyProcess(VK_NEXT, KeyState::Down, [&] { MoveInputProc(false); });
						Input.HitKeyProcess(VK_PRIOR, KeyState::Down, [&] { MoveInputProc(true); });

						if (Playing.Chart.IsBranchedChart) {
							Input.HitKeyProcess(VK_UP, KeyState::Down, [&] { BranchChangeProc(false); });
							Input.HitKeyProcess(VK_DOWN, KeyState::Down, [&] { BranchChangeProc(true); });
						}

						Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, StartInputProc);
						Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, StartInputProc);
						Input.HitKeyProcess(VK_RETURN, KeyState::Down, StartInputProc);
					}
					else {

						Playing.TrainingOffset = std::lerp(Playing.MemNowTime, Playing.Chart.RawNoteDatas[Playing.MeasureIndex].AbsTime, GetEasingRate(Playing.MeasureJump.GetRecordingTime() / Playing.MeasureJumpTime, ease::Base::In, ease::Line::Linear));

						if (Playing.MeasureJump.GetRecordingTime() >= Playing.MeasureJumpTime) {
							Playing.MeasureJump.End();
						}
					}

					return;
				}
			}
		}
	}

	if (Playing.Chart.SongBlankTime < NowTime && Playing.Chart.SongBlankTime + 128 > NowTime && !Playing.Chart.SongData.IsPlay()) {
		Playing.Chart.SongData.Play(TRUE);
	}
	else if (Playing.Chart.SongBlankTime + 5000 < NowTime && !Playing.Chart.SongData.IsPlay()) {
		if ((!Config.TrainingMode && !SongSelect.IsDanMode) || MultiRoom.MultiFlag) {
			if (!MultiRoom.MultiFlag || CheckStandby(Private.PlayerDatas, 3)) {
				NowScene = Scene::Result;
				return;
			}
			else if (Private.PlayerDatas[Private.MyIndex].Standby % MultiRoom.HostVal == 2) {
				Private.PlayerDatas[Private.MyIndex].Standby++;
				Send(DataType::List, Private.PlayerDatas[Private.MyIndex]);
			}
			return;
		}
		NowScene = Scene::Loading;
		return;
	}

	Playing.Chart.BalloonCount = 0;
	for (auto&& data : NoteDatas) {

		bool HitFlag = data.AbsTime < NowTime;

		if (data.Section && HitFlag) {
			Playing.Chart.BranchJudge = JudgeData();
			data.Section = false;
		}

		if (!Playing.Chart.LevelHold) {
			for (auto&& branchdata : Playing.Chart.BranchDatas) {
				if (branchdata.StartTime < NowTime && branchdata.Start) {

					if (Playing.Chart.Judge[0].Level != BranchLevel::None) {
						Playing.Chart.Judge[0].PrevLevel = Playing.Chart.Judge[0].Level;
					}

#define BRANCHCHECK(type) if (Playing.Chart.BranchJudge.type >= branchdata.Border[i] && BranchType::type == branchdata.Type) { Playing.Chart.Judge[0].Level = (BranchLevel)i; }

					for (int i = 0; i < (int)BranchLevel::Count; i++) {
						BRANCHCHECK(Accuracy);
						BRANCHCHECK(Roll);
						BRANCHCHECK(Score);
					}

#undef BRANCHCHECK

					if (Playing.Chart.Judge[0].PrevLevel != Playing.Chart.Judge[0].Level) {
						Playing.BranchLaneMove.Start();
					}

					branchdata.Start = false;
				}
			}
		}

		if (data.BranchLevel != BranchLevel::None) {
			if (data.BranchLevel != Playing.Chart.Judge[0].Level) { continue; }
		}

		if (data.LevelHold && HitFlag) {
			Playing.Chart.LevelHold = true;
			data.LevelHold = false;
		}

		if (data.GoGoStart && HitFlag) {
			Playing.Chart.NowGoGo = true;
		}
		if (data.GoGoEnd && HitFlag) {
			Playing.Chart.NowGoGo = false;
		}

		if (data.AbsTime - Config.JudgeBad > NowTime) { continue; }
		if (data.HitFlag) { continue; }

		const double _HitError = data.AbsTime - NowTime;
		const bool BadHit = _HitError > -Config.JudgeBad && _HitError < Config.JudgeBad;

		if (data.BigNoteTime != 0 && gameptr->Config.JudgeGood < NowTime - data.BigNoteTime) {
			Playing.JudgeNote(NowTime, data.NoteType - 2);
		}

		if (!data.HitFlag &&
			(data.NoteType >= '1' && data.NoteType <= '4') &&
			data.BigNoteTime == 0 &&
			_HitError < -Config.JudgeBad) {
			Playing.Chart.Judge[0].Hit(JudgeType::Bad, 0, '\0');
			Playing.Chart.BranchJudge.Hit(JudgeType::Bad, 0, '\0');
			Playing.Action(HitType::Empty);
			data.HitFlag = true;
		}

		if ((data.NoteType >= '5' && data.NoteType <= '6') &&
			HitFlag) {

			data.RollFlag = 1;

			if (data.RollEndTime < NowTime) {
				data.RollFlag = 2;
				data.HitFlag = true;
			}
		}

		if ((data.NoteType == '7' || data.NoteType == '9') &&
			HitFlag) {

			data.BalloonFlag = 1;
			Playing.Chart.BalloonCount = data.BalloonCount;

			if (data.RollEndTime < NowTime) {
				data.BalloonFlag = 2;
				data.HitFlag = true;
			}
		}
	}

	if (Config.AutoPlayFlag) {

		int RollCount = 0;
		NoteData* BalloonData = nullptr;

		bool NextImage = false;

		for (auto&& data : NoteDatas) {

			bool HitFlag = data.AbsTime < NowTime;
			bool IsHitNote = (data.NoteType >= '1' && data.NoteType <= '4');

			if (data.BranchLevel != BranchLevel::None) {
				if (data.BranchLevel != Playing.Chart.Judge[0].Level) { continue; }
			}

			if (data.RollFlag == 1) {
				++RollCount;
				NextImage = data.NoteType == '6';
			}

			if (data.BalloonFlag == 1) {
				BalloonData = &data;
			}

			if (HitFlag && !data.HitFlag && IsHitNote) {
				Playing.HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Good));
				Playing.Chart.Judge[0].Hit(JudgeType::Good, 0, data.NoteType);
				Playing.Chart.BranchJudge.Hit(JudgeType::Good, 0, '\0');
				switch (data.NoteType) {
				case '1':
					Skin.Base->Playing.SE.Don.Play();
					Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
					Playing.Action((HitType)(0 + Playing.Chart.AutoPlayLR * 2));
					break;
				case '2':
					Skin.Base->Playing.SE.Ka.Play();
					Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
					Playing.Action((HitType)(1 + Playing.Chart.AutoPlayLR * 2));
					break;
				case '3':
					Skin.Base->Playing.SE.Don.Play();
					Skin.Base->Playing.SE.Don.Play();
					Playing.Action(HitType::DonLeft);
					Playing.Action(HitType::DonRight);
					break;
				case '4':
					Skin.Base->Playing.SE.Ka.Play();
					Skin.Base->Playing.SE.Ka.Play();
					Playing.Action(HitType::KaLeft);
					Playing.Action(HitType::KaRight);
					break;
				}
				data.NoteType = '\0';
				data.HitFlag = true;
			}
		}

		const double WaitRollTimer = Playing.Chart.WaitRollTime.GetRecordingTime() / TimerType::microsecond;
		const double WaitRollTime = 1 / Config.RollSpeed;
		if (RollCount > 0 && WaitRollTimer > WaitRollTime) {
			Skin.Base->Playing.SE.Don.Play();
			Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
			Playing.Chart.Judge[0].Roll++;
			Playing.Chart.BranchJudge.Roll++;
			Playing.HitNote[0].Add(HitNoteData(NextImage ? '6' : '5', JudgeType::Roll));
			Playing.Chart.WaitRollTime.Start();
		}
		if (BalloonData != nullptr && WaitRollTimer > WaitRollTime) {
			Skin.Base->Playing.SE.Don.Play();
			Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
			Playing.Chart.Judge[0].Roll++;
			Playing.Chart.BranchJudge.Roll++;
			--BalloonData->BalloonCount;
			Playing.Chart.WaitRollTime.Start();
			if (BalloonData->BalloonCount <= 0) {
				gameptr->Skin.Base->Playing.SE.Balloon.Play();
				Playing.HitNote[0].Add(HitNoteData('3', JudgeType::Roll));
				BalloonData->NoteType = '0';
				BalloonData->HitFlag = true;
				BalloonData->BalloonFlag = 2;
				Playing.Chart.Judge[0].NoteType = '3';
			}
		}
	}

	else {

		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Don.Play();
			Playing.JudgeNote(NowTime, '1');
			Playing.Action(HitType::DonLeft);
			});
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Don.Play();
			Playing.JudgeNote(NowTime, '1');
			Playing.Action(HitType::DonRight);
			});
		Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Ka.Play();
			Playing.JudgeNote(NowTime, '2');
			Playing.Action(HitType::KaLeft);
			});
		Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Ka.Play();
			Playing.JudgeNote(NowTime, '2');
			Playing.Action(HitType::KaRight);
			});

	}

	if (SongSelect.IsDanMode) {

		for (uint i = 0; i < Playing.Chart.OriginalData.ExamDatas.size(); i++) {

			auto ExamData = Playing.Chart.OriginalData.ExamDatas[i];

#define EXAMVAL(type) case ExamTypes::type:\
Playing.Chart.ExamDatas[i].ExamVals = std::abs(((int)ExamData.PassVal[0] * (int)ExamData.Range) - (int)Playing.Chart.Judge[0].type);\
break;\

			switch (ExamData.ExamType) {
				EXAMVAL(Accuracy);
				EXAMVAL(Good);
				EXAMVAL(Ok);
				EXAMVAL(Bad);
				EXAMVAL(Score);
				EXAMVAL(Roll);
				EXAMVAL(HitNote);
				EXAMVAL(MaxCombo);
			}

			if (ExamData.Range == ExamRange::Less && Playing.Chart.ExamDatas[i].ExamVals <= 0) {
				if (!Playing.Chart.ExamDatas[i].IsFall) {
					Skin.Base->Playing.SE.DanFall.Play();
					Playing.Chart.ExamDatas[i].IsFall = true;
					Playing.Chart.IsFall = true;
				}
			}
		}
	}

	if (MultiRoom.MultiFlag) {

		if (Public.HitKey != HitType::Null) {

			if (Public.GetIndex <= Private.MyIndex) {
				Public.GetIndex++;
			}

			if (Public.HitKey >= HitType::DonLeft && Public.HitKey <= HitType::KaRight) {

				Playing.MiniTaikoFlash[(int)Public.HitKey + 4 * Public.GetIndex].Start();

				if (Public.Judge.HitJudge != JudgeType::None) {
					Playing.HitNote[Public.GetIndex].Add(HitNoteData(Public.Judge.NoteType, Public.Judge.HitJudge));
				}
			}

			Playing.Chart.Judge[Public.GetIndex] = Public.Judge;
		}
	}
}

void _Playing::JudgeNote(double nowtime, char type) {

	auto& NoteDatas = Chart.RawNoteDatas;
	auto& BranchJudge = Chart.BranchJudge;
	auto& Judge = Chart.Judge[0];

	int rollcount = 0;
	int ballooncount = 0;
	NoteData* balloondata = nullptr;

	bool NextImage = false;

	for (auto&& data : NoteDatas) {

		if (data.BranchLevel != BranchLevel::None) {
			if (data.BranchLevel != Judge.Level) { continue; }
		}
		
		if (data.HitFlag) {
			continue;
		}

		if (data.RollFlag == 1) {
			++rollcount;
			NextImage = data.NoteType == '6';
		}

		if (data.BalloonFlag == 1) {
			balloondata = &data;
		}

		if (data.BigNoteTime != 0) {
			if (gameptr->Config.JudgeGood < nowtime - data.BigNoteTime) {
				data.NoteType -= 2;
			}
			nowtime = data.BigNoteTime;
		}

		const double _HitError = data.AbsTime - nowtime;
		const bool GoodHit =
			_HitError > -gameptr->Config.JudgeGood && _HitError < gameptr->Config.JudgeGood;
		const bool OkHit =
			_HitError > -gameptr->Config.JudgeOk && _HitError < gameptr->Config.JudgeOk;
		const bool BadHit =
			_HitError > -gameptr->Config.JudgeBad && _HitError < gameptr->Config.JudgeBad;
		bool TypeMatch = type == data.NoteType;

		switch (data.NoteType) {
		case '3':
		case '4':
			TypeMatch = type == data.NoteType - 2;
			break;
		}

		if (!(BadHit && TypeMatch)) { continue; }

		switch (data.NoteType) {
		case '3':
		case '4':
			if (data.BigNoteTime == 0) {
				data.BigNoteTime = nowtime;
				return;
			}
		}

		const int addscore = Chart.AddScore;
		if (GoodHit) {
			HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Good));
			Judge.Hit(JudgeType::Good, addscore, type);
			BranchJudge.Hit(JudgeType::Good, addscore, '\0');
		}
		else if (OkHit) {
			HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Ok));
			Judge.Hit(JudgeType::Ok, addscore, type);
			BranchJudge.Hit(JudgeType::Ok, addscore, '\0');
		}
		else if (BadHit) {
			HitNote[0].Add(HitNoteData('\0', JudgeType::Bad));
			Judge.Hit(JudgeType::Bad, 0, '\0');
			BranchJudge.Hit(JudgeType::Bad, 0, '\0');
		}

		data.HitFlag = true;
		data.NoteType = '\0';

		return;
	}

	if (rollcount > 0) {
		HitNote[0].Add(HitNoteData(NextImage ? '6' : '5', JudgeType::Roll));
		Judge.Hit(JudgeType::Roll, 100, NextImage ? '6' : '5');
		BranchJudge.Hit(JudgeType::Roll, 100, '\0');
	}

	if (type == '1' && balloondata != nullptr) {
		--balloondata->BalloonCount;
		Judge.Hit(JudgeType::Roll, 100, '\0');
		BranchJudge.Hit(JudgeType::Roll, 100, '\0');
		if (balloondata->BalloonCount <= 0) {
			gameptr->Skin.Base->Playing.SE.Balloon.Play();
			HitNote[0].Add(HitNoteData('3', JudgeType::Roll));
			balloondata->NoteType = '0';
			balloondata->HitFlag = true;
			balloondata->BalloonFlag = 2;
			Judge.NoteType = '3';
		}
	}
}

void _Playing::BranchLaneClip(bool isclip, Pos2D<float> pos) {

	float clip[4] = {
		gameptr->Skin.Base->Playing.Image.Lane.Pos.X,
		gameptr->Skin.Base->Playing.Image.Lane.Pos.Y + pos.Y,
		gameptr->Skin.Base->Playing.Image.Lane.Size.Width / 2,
		gameptr->Skin.Base->Playing.Image.Lane.Size.Height / 2
	};

	if (isclip) {
		SetDrawArea(
			clip[0] - clip[2],
			clip[1] - clip[3],
			clip[0] + clip[2],
			clip[1] + clip[3]
		);
	}
	else {
		SetDrawArea(0, 0, gameptr->Skin.Info.Resolution.X, gameptr->Skin.Info.Resolution.Y);
	}
};

void _Playing::BranchLaneDraw(int index, Pos2D<float> pos) {

	if (!Chart.IsBranchedChart) { return; }

	auto&& Judge = Chart.Judge[index];

	Pos2D<float> SkinPos = {
			gameptr->Skin.Base->Playing.Image.Lane.Size.Width,
			gameptr->Skin.Base->Playing.Image.Lane.Size.Height
	};

	for (int i = 0; i < (int)BranchLevel::Count; i++) {

		Judge.Level = Judge.Level > BranchLevel::None ? Judge.Level : BranchLevel::Normal;
		int Dir = Judge.Level < Judge.PrevLevel ? 1 : -1;
		float Offset = 0.0f;
		if (BranchLaneMove.GetNowRecording()) {
			Offset = SkinPos.Y * -i + SkinPos.Y * (((int)Judge.Level + Dir) - GetEasingRate(BranchLaneMove.GetRecordingTime() / gameptr->Skin.Base->Playing.Config.BranchSlideTime, ease::Base::In, ease::Line::Sine) * Dir);
		}

		BranchLaneClip(true, pos);
		switch ((BranchLevel)i) {
		case BranchLevel::Normal:
			gameptr->Skin.Base->Playing.Image.NormalLane.Draw({ pos.X, pos.Y + Offset }, 0);
			break;
		case BranchLevel::Expert:
			gameptr->Skin.Base->Playing.Image.ExpertLane.Draw({ pos.X, pos.Y + Offset }, 0);
			break;
		case BranchLevel::Master:
			gameptr->Skin.Base->Playing.Image.MasterLane.Draw({ pos.X, pos.Y + Offset }, 0);
			break;
		}
		BranchLaneClip(false, pos);

	}
}

void _Playing::TitleDraw(std::string str, int strlen) {
	gameptr->Skin.Base->Playing.Font.Title.Draw(
		gameptr->Skin.Base->Playing.Config.TitlePos,
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		strlen,
		str
	);
}

void _Playing::SubTitleDraw(std::string str, int strlen) {
	gameptr->Skin.Base->Playing.Font.SubTitle.Draw(
		gameptr->Skin.Base->Playing.Config.SubTitlePos,
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		strlen,
		str
	);
}

void _Playing::ComboDraw(int index, Pos2D<float> pos) {
	ulonglong combo = Chart.Judge[index].Combo;

	int digit = std::digit(combo);

	float offset = gameptr->Skin.Base->Playing.Image.ComboNumber.Size.Width * (digit - 1) / 2;
	int i = 0;
	do {
		gameptr->Skin.Base->Playing.Image.ComboNumber.Draw({ offset, pos.Y }, combo % 10);
		combo /= 10;
		++i;
		offset -= gameptr->Skin.Base->Playing.Image.ComboNumber.Size.Width;
	} while (i < digit);
}

void _Playing::ScoreDraw(int index, Pos2D<float> pos) {
	ulonglong score = Chart.Judge[index].Score;

	int digit = std::digit(score);

	float offset = gameptr->Skin.Base->Playing.Image.ScoreNumber.Size.Width - (digit - 1) + digit;
	int i = 0;
	do {
		gameptr->Skin.Base->Playing.Image.ScoreNumber.Draw({ offset, pos.Y }, score % 10);
		score /= 10;
		++i;
		offset -= gameptr->Skin.Base->Playing.Image.ScoreNumber.Size.Width;
	} while (i < digit);
}

void _Playing::BalloonDraw(int val, Pos2D<float> pos) {
	int c = val;
	int digit = std::digit(c);

	float offset = gameptr->Skin.Base->Playing.Image.BalloonNumber.Size.Width - (digit - 1) + digit;
	int i = 0;
	do {
		gameptr->Skin.Base->Playing.Image.BalloonNumber.Draw({ offset, pos.Y }, c % 10);
		c /= 10;
		++i;
		offset -= gameptr->Skin.Base->Playing.Image.BalloonNumber.Size.Width;
	} while (i < digit);
}

void _Playing::NameDraw(std::string name, Pos2D<float> pos) {

	gameptr->Skin.Base->Playing.Font.PlayerName.Draw({
		gameptr->Skin.Base->Playing.Config.PlayerNamePos.X,
		gameptr->Skin.Base->Playing.Config.PlayerNamePos.Y + pos.Y },
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		GetStrlen(name, gameptr->Skin.Base->Playing.Font.PlayerName.Handle),
		name
		);
}

void _Playing::ProgressBarDraw(int index, Pos2D<float> pos) {

	gameptr->Skin.Base->Playing.Image.ProgressBar.Draw(pos, 0);

	if (!Chart.Judge[index].HitNote) { return; }

	double Ratio = ((double)Chart.Judge[index].Good + (double)Chart.Judge[index].Ok * 0.5) / Chart.AllNoteCount;
	float Width = gameptr->Skin.Base->Playing.Image.ProgressBar.Size.Width * Ratio;

	gameptr->Skin.Base->Playing.Image.ProgressBar.RectDraw(
		pos,
		{ 0,gameptr->Skin.Base->Playing.Image.ProgressBar.Size.Height },
		{ std::min(gameptr->Skin.Base->Playing.Image.ProgressBar.Size.Width, Width),
		gameptr->Skin.Base->Playing.Image.ProgressBar.Size.Height },
		1
	);
}

void _Playing::ExamProgressBarDraw() {

	for (uint i = 0; i < Chart.ExamDatas.size(); i++) {

		gameptr->Skin.Base->Playing.Image.ExamProgressBar.Draw({ 0,120.0f * i }, 0);

		double Ratio = (double)Chart.ExamDatas[i].ExamVals / (double)Chart.OriginalData.ExamDatas[i].PassVal[0];
		float Width = gameptr->Skin.Base->Playing.Image.ExamProgressBar.Size.Width * Ratio;

		if (!Chart.ExamDatas[i].IsFall) {
			gameptr->Skin.Base->Playing.Image.ExamProgressBar.RectDraw(
				{ 0,120.0f * i },
				{ 0, gameptr->Skin.Base->Playing.Image.ExamProgressBar.Size.Height },
				{ std::min(gameptr->Skin.Base->Playing.Image.ExamProgressBar.Size.Width, Width),
				gameptr->Skin.Base->Playing.Image.ExamProgressBar.Size.Height },
				1
			);
		}
	}
}

void _Playing::ExamValDraw() {

	for (uint i = 0; i < Chart.ExamDatas.size(); i++) {

		auto ExamData = Chart.OriginalData.ExamDatas[i];
		auto ExamVal = Chart.ExamDatas[i].ExamVals;
		bool IsFall = Chart.ExamDatas[i].IsFall;
		bool IsPass = ExamData.PassVal[0] <= ExamVal && ExamData.Range == ExamRange::More;
		std::string valstr = !IsFall ? std::to_string(ExamVal) : "0";
		std::string examname = ExamList[(int)ExamData.ExamType];

		gameptr->Skin.Base->Playing.Font.ExamName.Draw(
			{ gameptr->Skin.Base->Playing.Config.ExamNamePos.X,
			gameptr->Skin.Base->Playing.Config.ExamNamePos.Y + (120.0f * i) },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			GetStrlen(examname, gameptr->Skin.Base->Playing.Font.ExamName.Handle),
			examname
		);
		gameptr->Skin.Base->Playing.Font.ExamVal.Draw(
			{ gameptr->Skin.Base->Playing.Config.ExamValPos.X,
			gameptr->Skin.Base->Playing.Config.ExamValPos.Y + (120.0f * i) },
			GetColor(255, 255 * !IsFall, 255 * !IsFall * !IsPass),
			GetColor(0, 0, 0),
			GetStrlen(valstr, gameptr->Skin.Base->Playing.Font.ExamVal.Handle),
			valstr
		);
	}
}

void _Playing::Action(HitType type) {

	if ((int)type >= 0) {
		MiniTaikoFlash[(int)type].Start();
	}

	if (gameptr->Config.AutoPlayFlag) {
		Chart.Judge[0].Score = 0;
	}

	if (gameptr->MultiRoom.MultiFlag) {
		gameptr->Public.HitKey = type;
		gameptr->Public.Judge = Chart.Judge[0];
		gameptr->Public.GetIndex = gameptr->Private.MyIndex;
		gameptr->Send(DataType::Public, gameptr->Public);
		Chart.Judge[0].NoteType = '\0';
		Chart.Judge[0].HitJudge = JudgeType::None;
	}
}