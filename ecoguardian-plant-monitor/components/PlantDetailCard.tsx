import React from 'react';
import { PlantProfile } from '../types';
import { ThermometerIcon, DropletsIcon, SunIcon, ActivityIcon } from './Icons';

interface PlantDetailCardProps {
    plant: PlantProfile;
}

export const PlantDetailCard: React.FC<PlantDetailCardProps> = ({ plant }) => {
    return (
        <div className="bg-white/80 backdrop-blur-md rounded-2xl p-6 shadow-sm border border-slate-100 mb-8 mx-6 relative z-30 -mt-10">
            <div className="flex flex-col md:flex-row gap-6 items-start md:items-center justify-between">

                <div className="flex-1">
                    <h2 className="text-2xl font-bold text-slate-800 flex items-center gap-2 mb-2">
                        🌱 {plant.name}
                    </h2>
                    <p className="text-slate-600 leading-relaxed italic border-l-4 border-emerald-400 pl-4 py-1 bg-emerald-50/50 rounded-r-lg">
                        {plant.description}
                    </p>
                </div>

                <div className="grid grid-cols-2 sm:grid-cols-4 gap-4 w-full md:w-auto">
                    <div className="bg-rose-50 p-3 rounded-xl border border-rose-100 flex flex-col items-center min-w-[100px]">
                        <span className="text-rose-500 mb-1"><ThermometerIcon /></span>
                        <span className="text-xs text-rose-400 font-semibold uppercase">Temp</span>
                        <span className="font-bold text-slate-700">{plant.needs.temperature.min}-{plant.needs.temperature.max}°C</span>
                    </div>

                    <div className="bg-sky-50 p-3 rounded-xl border border-sky-100 flex flex-col items-center min-w-[100px]">
                        <span className="text-sky-500 mb-1"><DropletsIcon /></span>
                        <span className="text-xs text-sky-400 font-semibold uppercase">Air</span>
                        <span className="font-bold text-slate-700">{plant.needs.humidite.min}-{plant.needs.humidite.max}%</span>
                    </div>

                    <div className="bg-emerald-50 p-3 rounded-xl border border-emerald-100 flex flex-col items-center min-w-[100px]">
                        <span className="text-emerald-500 mb-1"><ActivityIcon /></span>
                        <span className="text-xs text-emerald-400 font-semibold uppercase">Sol</span>
                        <span className="font-bold text-slate-700">{plant.needs.humidite_sol.min}-{plant.needs.humidite_sol.max}%</span>
                    </div>

                    <div className="bg-amber-50 p-3 rounded-xl border border-amber-100 flex flex-col items-center min-w-[100px]">
                        <span className="text-amber-500 mb-1"><SunIcon /></span>
                        <span className="text-xs text-amber-400 font-semibold uppercase">Lumière</span>
                        <span className="font-bold text-slate-700">{plant.needs.luminosite.min}-{plant.needs.luminosite.max}%</span>
                    </div>
                </div>
            </div>
        </div>
    );
};
