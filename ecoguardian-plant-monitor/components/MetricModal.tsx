import React from 'react';
import { AreaChart, Area, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, ReferenceLine } from 'recharts';
import { PlantData } from '../types';

interface MetricModalProps {
  isOpen: boolean;
  onClose: () => void;
  title: string;
  dataKey: keyof PlantData;
  data: PlantData[];
  color: string;
  unit: string;
  min?: number;
  max?: number;
}

export const MetricModal: React.FC<MetricModalProps> = ({
  isOpen,
  onClose,
  title,
  dataKey,
  data,
  color,
  unit,
  min,
  max
}) => {
  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4">
      {/* Backdrop blur */}
      <div
        className="absolute inset-0 bg-black/40 backdrop-blur-sm transition-opacity"
        onClick={onClose}
      />

      {/* Modal Content */}
      <div className="relative bg-white w-full max-w-4xl rounded-2xl shadow-2xl p-6 md:p-8 animate-fade-in-up">
        <div className="flex justify-between items-center mb-6">
          <div>
            <h2 className="text-2xl font-bold text-slate-800">{title}</h2>
            <p className="text-slate-500">Historique détaillé</p>
          </div>
          <button
            onClick={onClose}
            className="p-2 rounded-full hover:bg-slate-100 text-slate-500 transition-colors"
          >
            <svg className="w-6 h-6" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M6 18L18 6M6 6l12 12" />
            </svg>
          </button>
        </div>

        <div className="h-[400px] w-full bg-slate-50 rounded-xl p-4 border border-slate-100">
          <ResponsiveContainer width="100%" height="100%">
            <AreaChart data={data}>
              {min !== undefined && <ReferenceLine y={min} stroke="red" strokeDasharray="3 3" label={{ position: 'right', value: 'Min', fill: 'red', fontSize: 12 }} />}
              {max !== undefined && <ReferenceLine y={max} stroke="red" strokeDasharray="3 3" label={{ position: 'right', value: 'Max', fill: 'red', fontSize: 12 }} />}
              <defs>
                <linearGradient id={`gradient-${dataKey}`} x1="0" y1="0" x2="0" y2="1">
                  <stop offset="5%" stopColor={color} stopOpacity={0.3} />
                  <stop offset="95%" stopColor={color} stopOpacity={0} />
                </linearGradient>
              </defs>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#e2e8f0" />
              <XAxis
                dataKey="time"
                tick={{ fill: '#64748b', fontSize: 12 }}
                tickLine={false}
                axisLine={false}
                minTickGap={30}
              />
              <YAxis
                tick={{ fill: '#64748b', fontSize: 12 }}
                tickLine={false}
                axisLine={false}
                domain={['auto', 'auto']}
              />
              <Tooltip
                contentStyle={{ borderRadius: '12px', border: 'none', boxShadow: '0 4px 6px -1px rgb(0 0 0 / 0.1)' }}
                labelStyle={{ color: '#64748b' }}
                formatter={(value: number) => [`${value} ${unit}`, title]}
              />
              <Area
                type="monotone"
                dataKey={dataKey as string}
                stroke={color}
                strokeWidth={3}
                fillOpacity={1}
                fill={`url(#gradient-${dataKey})`}
                animationDuration={1000}
              />
            </AreaChart>
          </ResponsiveContainer>
        </div>
      </div>
    </div>
  );
};