import React from 'react';

interface MetricCardProps {
  title: string;
  value: string | number;
  unit: string;
  subValue?: string;
  icon: React.ReactNode;
  colorClass: string;
  bgColorClass: string;
  onClick?: () => void;
  min?: number;
  max?: number;
}

export const MetricCard: React.FC<MetricCardProps> = ({
  title,
  value,
  unit,
  subValue,
  icon,
  colorClass,
  bgColorClass,
  onClick,
  min,
  max
}) => {
  const numValue = typeof value === 'number' ? value : parseFloat(value as string);
  const hasLimits = min !== undefined && max !== undefined && !isNaN(numValue);
  const isGood = hasLimits ? (numValue >= min! && numValue <= max!) : true;

  return (
    <div
      onClick={onClick}
      className={`bg-white rounded-2xl p-6 shadow-sm border ${isGood ? 'border-slate-100' : 'border-red-400 bg-red-50 animate-pulse'} flex items-center justify-between transition-all hover:shadow-lg hover:-translate-y-1 ${onClick ? 'cursor-pointer hover:border-emerald-200' : ''}`}
    >
      <div>
        <p className="text-slate-500 text-sm font-medium mb-1 uppercase tracking-wider">{title}</p>
        <div className="flex items-baseline gap-1">
          <span className={`text-3xl font-bold ${colorClass}`}>
            {value}
          </span>
          <span className="text-slate-400 font-medium">{unit}</span>
        </div>
        {subValue && (
          <p className="text-xs text-slate-400 mt-1 font-mono">{subValue}</p>
        )}

        {hasLimits && (
          <div className={`text-xs mt-1 font-medium flex items-center gap-1 ${isGood ? 'text-emerald-600' : 'text-red-500'}`}>
            {isGood ? '✅' : '⚠️'} Idéal: {min}-{max}{unit}
          </div>
        )}

        {onClick && <p className="text-xs text-slate-300 mt-2">Cliquez pour voir le détail</p>}
      </div>
      <div className={`p-4 rounded-full ${bgColorClass} ${colorClass}`}>
        {icon}
      </div>
    </div>
  );
};