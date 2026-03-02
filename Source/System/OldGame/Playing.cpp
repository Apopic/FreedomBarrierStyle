#include "Playing.h"
#include "GameSystem.h"

GameSystem *gameptr;

_Playing::_Playing(GameSystem *ptr) {
	::gameptr = ptr;
}

_Playing::~_Playing() {

}

void GameSystem::PlayingInit() {
	Playing.MiniTaikoFlash[0].End();
	Playing.MiniTaikoFlash[1].End();
	Playing.MiniTaikoFlash[2].End();
	Playing.MiniTaikoFlash[3].End();
	
	DiscordActivityChange(
		(Playing.Chart.OriginalData.Title + " / " + Playing.Chart.OriginalData.SubTitle).c_str(),
		nullptr,
		nullptr,
		true
	);
}

void GameSystem::PlayingEnd() {
	Playing.Chart.SongData.Delete();
}

void GameSystem::PlayingDraw() {
	
	auto& HitNote = Playing.HitNote;
	const double NowTime = Playing.ChartNowTime(Config.FrameCountTimer, Config.FastDrawRate);

	Skin.Base->Playing.Image.BackGround.Draw({ 0,0 });

	Skin.Base->Playing.Image.LaneFrame.Draw({ 0,0 });
	Skin.Base->Playing.Image.Lane.Draw({ 0,0 });
	
	Skin.Base->Playing.Image.Note.Draw({ 0,0 });

	const Pos2D<double>& NoteOrigin = {
		Skin.Base->Playing.Image.Note.Pos.X,
		Skin.Base->Playing.Image.Note.Pos.Y
	};

	auto GetNotePos = [&](TJANoteData& data)->Pos2D<double> {
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

		_ret = {
			_ret.X + NoteOrigin.X,
			_ret.Y + NoteOrigin.Y
		};

		return _ret;
		};

	{
		int i = HitNote.Index;
		const double JudgeUnderExplosionTime = Skin.Base->Playing.Config.JudgeUpperExplosionFrameTime * Skin.Base->Playing.Image.JudgeUnderExplosion.Div.X;

		for (int c = 0; c < HitNote.Size(); ++c) {
			auto& data = HitNote.Datas[i];
			if (!data.MoveTimer.GetNowRecording()) {
				data.MoveTimer.Start();
			}

			data.MoveElapsedTime = data.MoveTimer.GetRecordingTime() / TimerType::microsecond;

			if (data.JudgeUnderExplosion.IsActive && data.MoveElapsedTime < JudgeUnderExplosionTime) {

				uint drawindex = data.MoveElapsedTime / Skin.Base->Playing.Config.JudgeUpperExplosionFrameTime;

				drawindex += (2 * Skin.Base->Playing.Image.JudgeUnderExplosion.Div.X) * data.JudgeUnderExplosion.Big;

				if (data.JudgeUnderExplosion.Type == data.Ok) {
					drawindex += Skin.Base->Playing.Image.JudgeUnderExplosion.Div.X;
				}

				Skin.Base->Playing.Image.JudgeUnderExplosion.Draw({}, drawindex);
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
		auto& ProcNotes = Playing.Chart.RawNoteDatas;
		double _addms = ProcNotes[0].AbsTime;
		for (int i = 0, size = ProcNotes.size(); i < size; ++i) {
			TJANoteData& data = ProcNotes[i];
			
			if (data.AbsTime < NowTime) {
				data.BMFlag = true;
				Playing.Chart.NowBPM = data.BPM;
			}

			if (data.BpmChangeFlag) {
				if (data.BPM * data.Measure > 0) {
					for (int j = i + 1; j < size; ++j) {
						auto &jdata = ProcNotes[j];
						if (jdata.BpmChangeFlag && jdata.BPM * jdata.Measure < 0) {
							data.BpmChangeFlag = false;
							for (int k = j; k < size; ++k) {
								auto &kdata = ProcNotes[k];
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
	
	auto& NoteDatas = Playing.Chart.ProcNotes;
	const std::complex<double> n0 = { Skin.Base->Playing.Image.Note.Size.Width / 2, Skin.Base->Playing.Image.Note.Size.Height / 2 };
	const double n0r = std::abs(n0);
	const double narr[4] = {
		std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * -1 }),
		std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * -1 }),
		std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * 1 }),
		std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * 1 })
	};

	for (int match = NoteDatas.size() -1; match >= 0; --match) {
		Pos2D<double> firstmatchpos = GetNotePos(Playing.Chart.DrawNotes[match].First());
		Pos2D<double> lastmatchpos = GetNotePos(Playing.Chart.DrawNotes[match].Last());
		auto& TimeMatch = NoteDatas;
		if ((TimeMatch[match].First().get().AbsTime > NowTime && TimeMatch[match].Last().get().AbsTime < NowTime) &&
			!(InRange(firstmatchpos.X, firstmatchpos.Y) && InRange(lastmatchpos.X, lastmatchpos.Y))) {
			continue;
		}
		for (int i = NoteDatas[match].size() - 1; i >= 0; --i) {
			TJANoteData& data = NoteDatas[match][i];

			double NoteTheta = atan2(data.Scrolli, data.Scroll);

			NotePos = GetNotePos(data);

			if (data.BarlineDisplay) {
				if (InRange(NotePos.X, NotePos.Y)) {
					DrawLineAA(
						NotePos.X,
						NotePos.Y - 65,
						NotePos.X,
						NotePos.Y + 65,
						GetColor(255, 255, 255),
						1
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
					bool hiddenflag = Config.HiddenLevel > 0;
					bool suddenflag = Config.SuddenLevel > 0;
					if (hiddenflag || suddenflag) {
						double _abs = std::abs(std::complex<double>{ NotePos.X - NoteOrigin.X, NotePos.Y - NoteOrigin.Y })* (data.AbsTime < NowTime ? -1 : 1);
						double leveling = (Skin.Base->Playing.Image.Lane.Size.Width / DX_PI);
						double feedrange = (Skin.Base->Playing.Image.Lane.Size.Width / DX_TWO_PI);
						if (hiddenflag) {
							double _one = ((_abs - (leveling * Config.HiddenLevel)) / feedrange);
							if (_one > 1) {
								_one = 1;
							}
							if (_one < 0) {
								_one = 0;
							}
							NoteAlpha = 255 * _one;
						}
						if (suddenflag) {
							double _one = (((_abs - (Skin.Base->Playing.Image.Lane.Size.Width)) + (leveling * Config.SuddenLevel)) / feedrange);
							if (_one > 1) {
								_one = 1;
							}
							if (_one < 0) {
								_one = 0;
							}
							NoteAlpha = NoteAlpha * (1 - _one);
						}
					}
					if (NoteAlpha > 0) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, NoteAlpha);
					}
					Skin.Base->Playing.Image.Note.Draw(
						{
							(float)(NotePos.X - NoteOrigin.X),
							(float)(NotePos.Y - NoteOrigin.Y)
						},
						data.NoteType - 48
					);
					if (NoteAlpha > 0) {
						SetDrawBlendMode(0, 0);
					}
				}
				continue;
			}

			if (data.NoteType >= '5' &&
				data.NoteType <= '6') {
				const Pos2D<double>& cnote = NotePos;
				const Pos2D<double>& dnote = GetNotePos(Playing.Chart.RawNoteDatas[data.RollEndIndex]);

				bool DispFlag =
					InRange(cnote.X, cnote.Y) ||
					(data.AbsTime < NowTime && data.RollEndTime > NowTime) ||
					InRange(dnote.X, dnote.Y);

				if (DispFlag) {

					bool BigRollFlag = data.NoteType == '6';

					const std::complex<double>& cdnote = {dnote.X - cnote.X, dnote.Y - cnote.Y};

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
					NotePos = GetNotePos(Playing.Chart.RawNoteDatas[data.RollEndIndex]);
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
	}

	Skin.Base->Playing.Image.Base.Draw({ 0,0 });
	Skin.Base->Playing.Image.NamePlate.Draw({ 0,0 });
	Skin.Base->Playing.Image.MiniTaiko.Draw({ 0,0 });

	{
		int i = HitNote.Index;
		
		for (int c = 0; c < HitNote.Size(); ++c) {
			auto& data = HitNote.Datas[i];
			if (!data.MoveTimer.GetNowRecording()) {
				data.MoveTimer.Start();
			}

			if ((data.FlyingNote.IsActive && Config.HitNoteDisp) && data.MoveElapsedTime < data.FlyingNote.MoveTime()) {
				
				float _one = (data.MoveElapsedTime / data.FlyingNote.MoveTime());

				std::complex<float> _pos1 = { 800, -90 };
				std::complex<float> _pos2 = std::polar(280.f, (DX_PI_F / 2) + std::arg(_pos1));
				std::complex<float> _r = { (_pos1.real() / 2) + _pos2.real(), (_pos1.imag() / 2) + _pos2.imag() };
				float mem0arg = std::arg(_r);
				_r += { -800 , 90 };
				float mem1arg = std::arg(_r);
				_r = { (_pos1.real() / 2) + _pos2.real(), (_pos1.imag() / 2) + _pos2.imag() };
				float allarg = mem1arg - mem0arg;
				std::complex<float> _c1 = std::polar(std::abs(_r), allarg * _one + mem0arg);

				Pos2D<float> Pos = {
					_r.real() - _c1.real(),
					_r.imag() - _c1.imag()
				};

				Skin.Base->Playing.Image.Note.Draw(Pos, data.FlyingNote.Type - 48);

				
			}
			else {
				data.FlyingNote.IsActive = false;
			}

			if (!data.FlyingNote.IsActive &&
				!data.JudgeUnderExplosion.IsActive //&&
				) {
				data = HitNoteData();
			}

			++i;
			if (!(i < HitNote.Size())) {
				i = 0;
			}
		}
	}

	const ease::Line FlashType = ease::Line::Cubic;
	if (Playing.MiniTaikoFlash[0].GetNowRecording()) {
		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			255 * (1 - GetEasingRate(Playing.MiniTaikoFlash[0].GetRecordingTime() / Playing.MiniTaikoFlashTime, ease::Base::In, FlashType))
		);
		Skin.Base->Playing.Image.MiniTaiko_Don.Draw(
			{
				Skin.Base->Playing.Image.MiniTaiko_Don.Size.Width * -0.5f,
				0
			},
			0
		);
	}
	if (Playing.MiniTaikoFlash[1].GetNowRecording()) {
		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			255 * (1 - GetEasingRate(Playing.MiniTaikoFlash[1].GetRecordingTime() / Playing.MiniTaikoFlashTime, ease::Base::In, FlashType))
		);
		Skin.Base->Playing.Image.MiniTaiko_Ka.Draw(
			{
				Skin.Base->Playing.Image.MiniTaiko_Don.Size.Width * -0.5f,
				0
			},
			0
		);
	}
	if (Playing.MiniTaikoFlash[2].GetNowRecording()) {
		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			255 * (1 - GetEasingRate(Playing.MiniTaikoFlash[2].GetRecordingTime() / Playing.MiniTaikoFlashTime, ease::Base::In, FlashType))
		);
		Skin.Base->Playing.Image.MiniTaiko_Don.Draw(
			{
				Skin.Base->Playing.Image.MiniTaiko_Don.Size.Width * 0.5f,
				0
			},
			1
		);
	}
	if (Playing.MiniTaikoFlash[3].GetNowRecording()) {
		SetDrawBlendMode(
			DX_BLENDMODE_ALPHA,
			255 * (1 - GetEasingRate(Playing.MiniTaikoFlash[3].GetRecordingTime() / Playing.MiniTaikoFlashTime, ease::Base::In, FlashType))
		);
		Skin.Base->Playing.Image.MiniTaiko_Ka.Draw(
			{
				Skin.Base->Playing.Image.MiniTaiko_Ka.Size.Width * 0.5f,
				0
			},
			1
		);
	}

	SetDrawBlendMode(0, 0);

	if (Playing.Chart.Judge.Combo >= 3) {
		Playing.ComboDraw();
	}

	Skin.Base->Playing.Font.Title.Draw(
		Skin.Base->Playing.Config.TitlePos,
		GetColor(255,255,255),
		GetColor(0,0,0),
		Playing.Chart.OriginalData.PlayingTitleStrlen,
		Playing.Chart.OriginalData.Title
	);
	Skin.Base->Playing.Font.SubTitle.Draw(
		Skin.Base->Playing.Config.SubTitlePos,
		GetColor(255,255,255),
		GetColor(0,0,0),
		Playing.Chart.OriginalData.PlayingSubTitleStrlen,
		Playing.Chart.OriginalData.SubTitle
	);

	if (Config.ViewDebugData) {
		DrawFormatString(0, 0, GetColor(255, 255, 255), "\n\n\nNowTime:%lf\nBPM:%lf", NowTime / Playing.Chart.SongSpeed, Playing.Chart.NowBPM * Playing.Chart.SongSpeed);
	}
}

void GameSystem::PlayingProc() {

	auto& NoteDatas = Playing.Chart.ProcNotes;
	const double NowTime = Playing.ChartNowTime();
	
	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		NowScene = Scene::SongSelect;
		});

	Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
		Input.HitKeyProcess(VK_TAB, KeyState::Down, [&] {
			NowScene = Scene::Result;
		});
	});

	Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
		Input.HitKeyProcess('Q', KeyState::Down, [&] {
			NowScene = Scene::Loading;
		});
	});

	if (Playing.Chart.SongBlankTime < NowTime && Playing.Chart.SongBlankTime + 128 > NowTime && !Playing.Chart.SongData.IsPlay()) {
		Playing.Chart.SongData.Play();
	}
	else if (Playing.Chart.SongBlankTime + 5000 < NowTime && !Playing.Chart.SongData.IsPlay()) {
		NowScene = Scene::Result;
		return;
	}
	
	
	ulonglong size = NoteDatas.size();
	for (auto& item : NoteDatas) {
		if (!(item.First().get().AbsTime - Config.JudgeBad < NowTime && (item.Last().get().AbsTime + Config.JudgeBad > NowTime || item.Last().get().RollEndTime + Config.JudgeBad > NowTime))) {
			continue;
		}
		for (auto &refdata : item) {
			TJANoteData& data = refdata;
			if (data.AbsTime - Config.JudgeBad > NowTime) { continue; }
			if (data.HitFlag) {
				continue;
			}

			bool HitFlag = data.AbsTime < NowTime;
			const double _HitError = data.AbsTime - NowTime;
			const bool BadHit = _HitError > -Config.JudgeBad && _HitError < Config.JudgeBad;

			// BigNote
			if (data.BigNoteTime != 0 && gameptr->Config.JudgeGood < NowTime - data.BigNoteTime) {
				Playing.JudgeNote(NowTime, data.NoteType - 2);
			}

			// Bad
			if (!data.HitFlag &&
				(data.NoteType >= '1' && data.NoteType <= '4') &&
				data.BigNoteTime == 0 &&
				_HitError < -Config.JudgeBad) {
				Playing.Chart.Judge.Combo = 0;
				++Playing.Chart.Judge.Bad;
				++Playing.Chart.Judge.HitNote;
				data.HitFlag = true;
				DiscordActivityChange(
					nullptr,
					Playing.ActivityDispJudges().c_str(),
					nullptr,
					false
				);
			}

			// Roll
			if ((data.NoteType >= '5' && data.NoteType <= '6') &&
				HitFlag) {

				data.RollFlag = 1;

				if (data.RollEndTime < NowTime) {
					data.RollFlag = 2;
					data.HitFlag = true;
				}
			}

			// Balloon
			if ((data.NoteType == '7' || data.NoteType == '9') &&
				HitFlag) {

				data.BalloonFlag = 1;

				if (data.RollEndTime < NowTime) {
					data.BalloonFlag = 2;
					data.HitFlag = true;
				}
			}
		}
	}
	if (Playing.Chart.Judge.HitNote != 0) {
		Playing.Chart.Judge.Accurecy =
			((Playing.Chart.Judge.Good / (double)Playing.Chart.Judge.HitNote) +
			((Playing.Chart.Judge.Ok / (double)Playing.Chart.Judge.HitNote) * 0.5)) *
			100;
	}

	if (Config.AutoPlayFlag) {

		int RollCount = 0;
		TJANoteData* BalloonData = nullptr;

		bool NextImage = false;

		for (auto& item : NoteDatas) {
			if (!(item.First().get().AbsTime - Config.JudgeBad < NowTime && (item.Last().get().AbsTime + Config.JudgeBad > NowTime || item.Last().get().RollEndTime + Config.JudgeBad > NowTime))) {
				continue;
			}
			for (auto &refdata : item) {
				TJANoteData& data = refdata;
				bool HitFlag = data.AbsTime < NowTime;
				bool IsHitNote = (data.NoteType >= '1' && data.NoteType <= '4');

				if (data.RollFlag == 1) {
					++RollCount;
					NextImage = data.NoteType == '6';
				}

				if (data.BalloonFlag == 1) {
					BalloonData = &data;
				}

				if (HitFlag && !data.HitFlag && IsHitNote) {
					switch (data.NoteType) {
						case '1':
							Skin.Base->Playing.SE.Don.Play();
							Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
							Playing.MiniTaikoFlash[Playing.Chart.AutoPlayLR * 2].Start();
							Playing.HitNote.Add(HitNoteData('1', HitNoteData::Good));
							break;
						case '2':
							Skin.Base->Playing.SE.Ka.Play();
							Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
							Playing.MiniTaikoFlash[(Playing.Chart.AutoPlayLR * 2) + 1].Start();
							Playing.HitNote.Add(HitNoteData('2', HitNoteData::Good));
							break;
						case '3':
							Skin.Base->Playing.SE.Don.Play();
							Skin.Base->Playing.SE.Don.Play();
							Playing.MiniTaikoFlash[0].Start();
							Playing.MiniTaikoFlash[2].Start();
							Playing.HitNote.Add(HitNoteData('3', HitNoteData::Good));
							break;
						case '4':
							Skin.Base->Playing.SE.Ka.Play();
							Skin.Base->Playing.SE.Ka.Play();
							Playing.MiniTaikoFlash[1].Start();
							Playing.MiniTaikoFlash[3].Start();
							Playing.HitNote.Add(HitNoteData('4', HitNoteData::Good));
							break;
					}
					++Playing.Chart.Judge.Combo;
					if (Playing.Chart.Judge.Combo > Playing.Chart.Judge.MaxCombo) { ++Playing.Chart.Judge.MaxCombo; }
					++Playing.Chart.Judge.Good;
					++Playing.Chart.Judge.HitNote;
					Playing.Chart.Judge.Score += Playing.Chart.AddScore;
					data.NoteType = '\0';
					data.HitFlag = true;
				}
			}
		}

		const double WaitRollTimer = Playing.Chart.WaitRollTime.GetRecordingTime() / TimerType::microsecond;
		const double WaitRollTime = 1 / Config.RollSpeed;
		if (RollCount > 0 && WaitRollTimer > WaitRollTime) {
			++Playing.Chart.Judge.Roll;
			Playing.Chart.Judge.Score += 100;
			Skin.Base->Playing.SE.Don.Play();
			Playing.MiniTaikoFlash[Playing.Chart.AutoPlayLR * 2].Start();
			Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
			Playing.Chart.WaitRollTime.Start();
			Playing.HitNote.Add(HitNoteData(NextImage ? '6' : '5', HitNoteData::Roll));
		}

		if (BalloonData != nullptr && WaitRollTimer > WaitRollTime) {
			--BalloonData->BalloonCount;
			++Playing.Chart.Judge.Roll;
			Playing.Chart.Judge.Score += 100;
			Skin.Base->Playing.SE.Don.Play();
			Playing.MiniTaikoFlash[Playing.Chart.AutoPlayLR * 2].Start();
			if (!(BalloonData->BalloonCount > 0)) {
				Skin.Base->Playing.SE.Balloon.Play();
				Playing.HitNote.Add(HitNoteData('3', HitNoteData::Roll));
				BalloonData->NoteType = '0';
				BalloonData->HitFlag = true;
				BalloonData->BalloonFlag = 2;
			}
			Playing.Chart.AutoPlayLR = !Playing.Chart.AutoPlayLR;
			Playing.Chart.WaitRollTime.Start();
		}
	}
	else {
		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Don.Play();
			Playing.MiniTaikoFlash[0].Start();
			Playing.JudgeNote(NowTime, '1');
			});
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Don.Play();
			Playing.MiniTaikoFlash[2].Start();
			Playing.JudgeNote(NowTime, '1');
			});
		Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Ka.Play();
			Playing.MiniTaikoFlash[1].Start();
			Playing.JudgeNote(NowTime, '2');
			});
		Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] {
			Skin.Base->Playing.SE.Ka.Play();
			Playing.MiniTaikoFlash[3].Start();
			Playing.JudgeNote(NowTime, '2');
			});
	}

}

void ChartStreamData::Init() {
	ProcNotes.clear();
	DrawNotes.clear();
	RawNoteDatas.clear();
	SongData.Delete();
	NowTime.End();
	OriginalData = ChartData();
	ScrollType = ScrollType::Normal;
	Judge = JudgeData();
	JudgeBranch = JudgeData();
	AutoPlayLR = false;
}

void _Playing::JudgeNote(double nowtime, char type) {

	const uint size = Chart.ProcNotes.size();
	auto& NoteDatas = Chart.ProcNotes;
	auto& Judge = Chart.Judge;
	
	int rollcount = 0;
	int ballooncount = 0;
	TJANoteData* balloondata = nullptr;

	bool NextImage = false;

	for (auto& item : NoteDatas) {
		if (!(item.First().get().AbsTime - gameptr->Config.JudgeBad < nowtime && (item.Last().get().AbsTime + gameptr->Config.JudgeBad > nowtime || item.Last().get().RollEndTime + gameptr->Config.JudgeBad > nowtime))) {
			continue;
		}
		for (auto &refdata : item) {
			TJANoteData& data = refdata;
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
			const bool GoodHit = _HitError > -gameptr->Config.JudgeGood && _HitError < gameptr->Config.JudgeGood;
			const bool OkHit = _HitError > -gameptr->Config.JudgeOk && _HitError < gameptr->Config.JudgeOk;
			const bool BadHit = _HitError > -gameptr->Config.JudgeBad && _HitError < gameptr->Config.JudgeBad;
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
				HitNote.Add(HitNoteData(data.NoteType, HitNoteData::Good));
				++Judge.Good;
				++Judge.Combo;
				Judge.Score += addscore;
			}
			else if (OkHit) {
				HitNote.Add(HitNoteData(data.NoteType, HitNoteData::Ok));
				++Judge.Ok;
				++Judge.Combo;
				Judge.Score += addscore / 2;
			}
			else if (BadHit) {
				HitNote.Add(HitNoteData(data.NoteType, HitNoteData::Bad));
				++Judge.Bad;
				Judge.Combo = 0;
			}

			if (Judge.Combo > Judge.MaxCombo) { ++Judge.MaxCombo; }
			++Judge.HitNote;
			data.HitFlag = true;
			data.NoteType = '0';

			gameptr->DiscordActivityChange(
				nullptr,
				ActivityDispJudges().c_str(),
				nullptr,
				false
			);

			return;
		}
	}

	if (rollcount > 0) {
		HitNote.Add(HitNoteData(NextImage ? '6' : '5', HitNoteData::Roll));
		++Judge.Roll;
		Judge.Score += 100;
	}

	if (type == '1' && balloondata != nullptr) {
		--balloondata->BalloonCount;
		++Judge.Roll;
		Judge.Score += 100;
		if (!(balloondata->BalloonCount > 0)) {
			gameptr->Skin.Base->Playing.SE.Balloon.Play();
			HitNote.Add(HitNoteData('3', HitNoteData::Roll));
			balloondata->NoteType = '0';
			balloondata->HitFlag = true;
			balloondata->BalloonFlag = 2;
		}
	}
}

void _Playing::ComboDraw() {
	ulonglong combo = Chart.Judge.Combo;

	int digit = std::digit(combo);

	float offset = gameptr->Skin.Base->Playing.Image.ComboNumber.Size.Width * (digit - 1) / 2;
	int i = 0;
	do {
		gameptr->Skin.Base->Playing.Image.ComboNumber.Draw({offset, 0}, combo % 10);
		combo /= 10;
		++i;
		offset -= gameptr->Skin.Base->Playing.Image.ComboNumber.Size.Width;
	} while (i < digit);
}

