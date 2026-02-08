import matplotlib.pyplot as plt
import numpy as np
group_names = ['Stock', 'Bond', 'Gold']
group_size = [804242, 181852, 132840]
subgroup_names = ['TIGERs&p500', 'QQQM', 'koreatop10', 'ACEUSA10years', 'SGOV', 'Gold']
subgroup_size = [652080, 123572, 28590, 31620, 150232, 132840]
a, b, c = [plt.cm.Reds, plt.cm.Blues, plt.cm.YlOrBr]
outer_colors = []
outer_colors += [a(x) for x in np.linspace(0.3, 0.9, 3)]
outer_colors += [b(x) for x in np.linspace(0.3, 0.9, 2)]
outer_colors += [c(0.6)]
inner_colors = [a(0.6), b(0.6), c(0.6)]
fig, ax = plt.subplots(figsize=(14, 10))
ax.axis('equal')
mypie, _ = ax.pie(group_size, radius=1.0, labels=group_names, labeldistance=0.6, colors=inner_colors, textprops={'fontsize': 12}, wedgeprops=dict(width=0.4, edgecolor='white'))
mypie2, _ = ax.pie(subgroup_size, radius=1.4, labels=None, colors=outer_colors, wedgeprops=dict(width=0.4, edgecolor='white'))
for i, w in enumerate(mypie2):
    ang = (w.theta2 - w.theta1)/2. + w.theta1
    rad = 1.35 if i % 2 == 0 else 1.55
    x = rad * np.cos(np.deg2rad(ang))
    y = rad * np.sin(np.deg2rad(ang))
    font_size = 12 if subgroup_size[i]/sum(subgroup_size) > 0.02 else 10
    ax.text(x, y, subgroup_names[i], ha='center', va='center', fontsize=font_size, fontweight='bold')
plt.title('Asset Allocation Breakdown', pad=20, fontsize=16)
textstr = ''
total = sum(group_size)
textstr += f'Total: {total:,} won\n\n'
textstr += f'Stock: {group_size[0]/total*100:.1f}%\n'
textstr += f'  - TIGERs&p500: {subgroup_size[0]/total*100:.1f}%\n'
textstr += f'  - QQQM: {subgroup_size[1]/total*100:.1f}%\n'
textstr += f'  - koreatop10: {subgroup_size[2]/total*100:.1f}%\n'
textstr += f'Bond: {group_size[1]/total*100:.1f}%\n'
textstr += f'  - ACEUSA10years: {subgroup_size[3]/total*100:.1f}%\n'
textstr += f'  - SGOV: {subgroup_size[4]/total*100:.1f}%\n'
textstr += f'Gold: {group_size[2]/total*100:.1f}%\n'
plt.gcf().text(0.02, 0.5, textstr, fontsize=12, bbox=dict(facecolor='white', alpha=0.5))
plt.subplots_adjust(left=0.3)
plt.savefig('chart.png')
